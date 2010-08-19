/* gccxml_cc1plus - A GCC parser patched for XML dumps of translation units
   Copyright (C) 2002-2007 Kitware, Inc., Insight Consortium

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the

  Free Software Foundation, Inc.
  51 Franklin Street, Fifth Floor
  Boston, MA  02110-1301  USA

*/

/* Organization of this source:
   The entry point is do_xml_output(), which is called from the end of
   finish_translation_unit() in semantics.c.

   xml_output_* functions are called to actually write the XML output.

   xml_print_*_attribute functions are used to write out the XML attributes
   for common attribute name/pair values.
*/

/* Use GCC_XML_GCC_VERSION to modify code based on the version of GCC
   in which we are being built.  This is set in the patched version of
   cp-tree.h.  The format is 0xMMmmpp, where MM is the major version
   number, mm is the minor version number, and pp is the patch level.
   Examples:  gcc 3.0.4 = 0x030004
              gcc 3.2.0 = 0x030200  */

#include "config.h"
#include "system.h"
#include "sys/stat.h"

/* GCC 3.4 and above need these headers here.  The GCC-XML patches for
   these versions define GCC_XML_GCC_VERSION in config.h instead of
   cp-tree.h, so the macro is available here.  The patches for older
   versions may provide the macro in cp-tree.h, but in that case
   we don't need these headers anyway.  */
#if defined(GCC_XML_GCC_VERSION) && (GCC_XML_GCC_VERSION >= 0x030400)
# include "coretypes.h"
# include "tm.h"
#endif

#include "tree.h"
#include "cp-tree.h"
#include "decl.h"
#include "rtl.h"
#include "varray.h"

#include "splay-tree.h"

#include "demangle.h"

#include "tree-iterator.h"

#include "toplev.h" /* ident_hash */

#define GCC_XML_C_VERSION "$Revision: 1.135 $"

/*--------------------------------------------------------------------------*/
/* Data structures for the actual XML dump.  */

/* A "dump node" corresponding to a particular tree node.  */
typedef struct xml_dump_node
{
  /* The index for the node.  */
  unsigned int index;

  /* Whether the node is complete.  */
  unsigned int complete;
} *xml_dump_node_p;

/* A node on the queue of dump nodes.  */
typedef struct xml_dump_queue
{
  /* The queued tree node.  */
  tree tree_node;

  /* The corresponding dump node.  */
  xml_dump_node_p dump_node;

  /* The next node in the queue.  */
  struct xml_dump_queue *next;
} *xml_dump_queue_p;

/* A node on the queue of file names.  */
typedef struct xml_file_queue
{
  /* The queued file name.  */
  splay_tree_node tree_node;

  /* The next node in the queue.  */
  struct xml_file_queue *next;
} *xml_file_queue_p;

/* Dump control structure.  A pointer one instance of this is passed
   to nearly every function.  */
typedef struct xml_dump_info
{
  /* Output file stream of dump.  */
  FILE* file;

  /* Which pass of the loop we are doing (1=complete or 0=incomplete).  */
  int require_complete;

  /* Next available index to identify node.  */
  unsigned int next_index;

  /* The first node in the queue of complete nodes.  */
  xml_dump_queue_p queue;

  /* The last node in the queue of complete nodes.  */
  xml_dump_queue_p queue_end;

  /* List of free xml_dump_queue nodes.  */
  xml_dump_queue_p queue_free;

  /* All nodes that have been encountered.  */
  splay_tree dump_nodes;

  /* Index of the next available file queue position.  */
  unsigned int file_index;

  /* The first file in the queue of filenames.  */
  xml_file_queue_p file_queue;

  /* The last file in the queue of filenames.  */
  xml_file_queue_p file_queue_end;

  /* All files that have been queued.  */
  splay_tree file_nodes;
} *xml_dump_info_p;

/*--------------------------------------------------------------------------*/
/* Data structures for generating documentation.  */

typedef struct xml_document_subelement_s xml_document_subelement;
typedef struct xml_document_attribute_s xml_document_attribute;
typedef struct xml_document_element_s xml_document_element;
typedef struct xml_document_info_s xml_document_info;
typedef xml_document_subelement* xml_document_subelement_p;
typedef xml_document_attribute* xml_document_attribute_p;
typedef xml_document_element* xml_document_element_p;
typedef xml_document_info* xml_document_info_p;

/* Maximum document specification sizes.  */
enum { xml_document_max_elements = 256,
       xml_document_max_attributes = 64,
       xml_document_max_subelements = 256 };

/* Document format types.  */
typedef enum xml_document_format_e
{ xml_document_format_dtd,
  xml_document_format_schema
} xml_document_format;

/* The modes of attribute use.  */
typedef enum xml_document_attribute_use_e
{ xml_document_attribute_use_optional,
  xml_document_attribute_use_required
} xml_document_attribute_use;
static const char* xml_document_dtd_uses[] = {"IMPLIED", "REQUIRED"};
static const char* xml_document_schema_uses[] = {"optional", "required"};

/* The set of attribute types.  */
typedef enum xml_document_attribute_type_e
{ xml_document_attribute_type_id,
  xml_document_attribute_type_idref,
  xml_document_attribute_type_idrefs,
  xml_document_attribute_type_integer,
  xml_document_attribute_type_boolean,
  xml_document_attribute_type_string,
  xml_document_attribute_type_enum_access
} xml_document_attribute_type;
static const char* xml_document_dtd_types[] =
{"ID", "IDREF", "IDREFS", "CDATA", "CDATA", "CDATA",
 "(public|protected|private)"};
static const char* xml_document_schema_types[] =
{"xs:ID", "xs:IDREF", "xs:IDREFS", "xs:integer", "xs:boolean", "xs:string",
 "????"};

/* Represent one element attribute.  */
struct xml_document_attribute_s
{
  /* The name of the attribute.  */
  const char* name;

  /* The type of the attribute.  */
  xml_document_attribute_type type;

  /* Usage requirements in the containing element (optional, required).  */
  xml_document_attribute_use use;

  /* The value of the attribute, if any.  When the usage is required
     this specifies the required value.  When the usage is optional
     this specifies the default value.  */
  const char* value;
};

/* Represent a reference to a nested element.  */
struct xml_document_subelement_s
{
  /* The element that may be nested.  */
  xml_document_element_p element;

  /* Whether the subelement is required to be present (at least once).  */
  int required;

  /* Whether the subelement is allowed to be repeated (more than once).  */
  int repeatable;
};

/* Represent an element specification.  */
struct xml_document_element_s
{
  /* The name of the element.  */
  const char* name;

  /* The attribute specification.  */
  int num_attributes;
  xml_document_attribute attributes[xml_document_max_attributes];

  /* The subelement specification.  */
  int num_subelements;
  xml_document_subelement subelements[xml_document_max_subelements];
};

/* Represent a full document specification.  */
struct xml_document_info_s
{
  /* The set of allowed elements.  The first one is the root which
     references the others.  */
  int num_elements;
  xml_document_element elements[xml_document_max_elements];

  /* The format of the documentation to be generated.  */
  xml_document_format format;

  /* Output file stream for document.  */
  FILE* file;
};

static void
xml_document_add_attribute(xml_document_element_p element,
                           const char* name,
                           xml_document_attribute_type type,
                           xml_document_attribute_use use,
                           const char* value)
{
  xml_document_attribute_p attribute =
    &element->attributes[element->num_attributes++];
  attribute->name = name;
  attribute->type = type;
  attribute->use = use;
  attribute->value = value;
}

static xml_document_element_p
xml_document_add_subelement(xml_document_info_p xdi,
                            xml_document_element_p parent,
                            int required, int repeatable)
{
  xml_document_element_p element = &xdi->elements[xdi->num_elements++];
  xml_document_subelement_p subelement =
    &parent->subelements[parent->num_subelements++];
  subelement->element = element;
  subelement->required = required;
  subelement->repeatable = repeatable;
  return element;
}

/*--------------------------------------------------------------------------*/
/* Dump utility declarations.  */

void do_xml_output PARAMS ((const char*));
void do_xml_document PARAMS ((const char*, const char*));

static int xml_add_node PARAMS((xml_dump_info_p, tree, int));
static void xml_dump PARAMS((xml_dump_info_p));
static int xml_queue_incomplete_dump_nodes PARAMS((splay_tree_node, void*));
static void xml_dump_tree_node PARAMS((xml_dump_info_p, tree, xml_dump_node_p));
static void xml_dump_files PARAMS((xml_dump_info_p));

static void xml_add_start_nodes PARAMS((xml_dump_info_p, const char*));

static const char* xml_get_encoded_string PARAMS ((tree));
static const char* xml_get_encoded_string_from_string PARAMS ((const char*));
static tree xml_get_encoded_identifier_from_string PARAMS ((const char*));
static const char* xml_escape_string PARAMS ((const char* in_str));
static int xml_fill_all_decls(struct cpp_reader*, hashnode, const void*);

#if defined(GCC_XML_GCC_VERSION) && (GCC_XML_GCC_VERSION >= 0x030100)
# include "diagnostic.h"
#else
extern int errorcount;
#endif

/* Switch to 1 to enable debugging of dump node selection.  */
#if 0
# define xml_add_node(xdi, node, complete)                                    \
   (fprintf(stderr, "Adding node at line %d\n", __LINE__),                    \
    xml_add_node(xdi, node, complete))
#endif

/* Get the revision number of this source file.  */
const char* xml_get_xml_c_version()
{
  const char* revision = GCC_XML_C_VERSION;
  char* version = (char*)xmalloc(strlen(revision)+1);
  const char* in=revision;
  char* out = version;
  while(*in && *in != ':') { ++in; }
  if(*in) { ++in; }
  while(*in && *in == ' ') { ++in; }
  while(*in && *in != ' ' && *in != '$')
    {
    *out++ = *in++;
    }
  *out = 0;
  return version;
}

/* Main XML output function.  Called by parser at the end of a translation
   unit.  Walk the entire translation unit starting at the global
   namespace.  Output all declarations.  */
void
do_xml_output (const char* filename)
{
  FILE* file;
  struct xml_dump_info xdi;

  /* Do not dump if errors occurred during parsing.  */
  if(errorcount)
    {
    /* Delete any existing output file.  */
    unlink(filename);
    return;
    }

  /* Fill in the all_decls member we added to each scope.  */
  ht_forall(ident_hash, xml_fill_all_decls, 0);

  /* Open the XML output file.  */
  file = fopen (filename, "w");
  if (!file)
    {
    error ("could not open xml-dump file `%s'", filename);
    return;
    }

  /* Prepare dump.  */
  xdi.file = file;
  xdi.queue = 0;
  xdi.queue_end = 0;
  xdi.queue_free = 0;
  xdi.next_index = 1;
  xdi.dump_nodes = splay_tree_new (splay_tree_compare_pointers, 0,
                                   (splay_tree_delete_value_fn) &free);
  xdi.file_queue = 0;
  xdi.file_queue_end = 0;
  xdi.file_index = 0;
  xdi.file_nodes = splay_tree_new (splay_tree_compare_pointers, 0, 0);
  xdi.require_complete = 1;

  /* Add the starting nodes for the dump.  */
  if (flag_xml_start)
    {
    /* Use the specified starting locations.  */
    xml_add_start_nodes (&xdi, flag_xml_start);
    }
  else
    {
    /* No start specified.  Use global namespace.  */
    xml_add_node (&xdi, global_namespace, 1);

    /* Also add std namespace because it is not always referenced.  */
    if(std_node)
      {
      xml_add_node (&xdi, std_node, 1);
      }
    }

  /* Start dump.  */
  fprintf (file, "<?xml version=\"1.0\"?>\n");
  fprintf (file, "<GCC_XML");
#if defined(GCCXML_VERSION_FULL)
  fprintf (file, " version=\"" GCCXML_VERSION_FULL "\"");
#endif
  fprintf (file, " cvs_revision=\"%s\"", xml_get_xml_c_version());
  fprintf (file, ">\n");

  /* Dump the complete nodes.  */
  xml_dump (&xdi);

  /* Queue all the incomplete nodes.  */
  splay_tree_foreach (xdi.dump_nodes,
                      &xml_queue_incomplete_dump_nodes, &xdi);

  /* Dump the incomplete nodes.  */
  xdi.require_complete = 0;
  xml_dump (&xdi);

  /* Dump the filename queue.  */
  xml_dump_files (&xdi);

  /* Finish dump.  */
  fprintf (file, "</GCC_XML>\n");

  /* Clean up.  */
  {
  xml_dump_queue_p dq = xdi.queue_free;
  while(dq)
    {
    xml_dump_queue_p nq = dq->next;
    free(dq);
    dq = nq;
    }
  }
  splay_tree_delete (xdi.dump_nodes);
  splay_tree_delete (xdi.file_nodes);
  fclose (file);
}

/* Return the xml_dump_node corresponding to tree node T.  If none exists,
   one will be created with an index of 0.  */
static xml_dump_node_p
xml_get_dump_node(xml_dump_info_p xdi, tree t)
{
  /* See if the node has already been inserted.  */
  splay_tree_node n = splay_tree_lookup (xdi->dump_nodes, (splay_tree_key) t);
  if(!n)
    {
    /* Need to add the node, create it.  */
    xml_dump_node_p v =
      (xml_dump_node_p) xmalloc (sizeof (struct xml_dump_node));

    /* Initialize it.  */
    v->index = 0;
    v->complete = 0;

    /* Insert the node.  */
    n = splay_tree_insert (xdi->dump_nodes, (splay_tree_key) t,
                           (splay_tree_value) v);
    }

  /* Return a pointer to the dump node.  */
  return (xml_dump_node_p)n->value;
}

/* Queue the given tree node for output as a complete node.  */
static void
xml_queue_node (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  xml_dump_queue_p dq;

  /* Obtain a new queue node.  */
  if (xdi->queue_free)
    {
    dq = xdi->queue_free;
    xdi->queue_free = dq->next;
    }
  else
    {
    dq = (xml_dump_queue_p) xmalloc (sizeof (struct xml_dump_queue));
    }

  /* Point the queue node at its corresponding tree node and dump node.  */
  dq->next = 0;
  dq->tree_node = t;
  dq->dump_node = dn;

  /* Add it to the end of the queue.  */
  if (!xdi->queue_end)
    {
    xdi->queue = dq;
    }
  else
    {
    xdi->queue_end->next = dq;
    }
  xdi->queue_end = dq;
}

/* Add tree node N to those encountered.  Return its index.  */
static int
xml_add_node_real (xml_dump_info_p xdi, tree n, int complete)
{
  /* Get the dump node for this tree node.  */
  xml_dump_node_p dn = xml_get_dump_node (xdi, n);
  if (dn->index)
    {
    /* Node was already encountered.  See if it is now complete.  */
    if(complete && !dn->complete)
      {
      /* Node is now complete, but wasn't before.  Queue it.  */
      dn->complete = 1;
      xml_queue_node (xdi, n, dn);
      }
    /* Return node's index.  */
    return dn->index;
    }

  /* This is a new node.  Assign it an index.  */
  dn->index = xdi->next_index++;
  dn->complete = complete;
  if(complete || !xdi->require_complete)
    {
    /* Node is complete.  Queue it.  */
    xml_queue_node (xdi, n, dn);
    }

  if(!xdi->require_complete && complete)
    {
    error ("XML dump bug: complete node added during incomplete phase.\n");
    }

  /* Return node's index.  */
  return dn->index;
}

/* Called for each node in the splay tree of dump nodes.  Queues
   a dump node if it is incomplete.  */
int xml_queue_incomplete_dump_nodes (splay_tree_node n, void* in_xdi)
{
  tree key = (tree)n->key;
  xml_dump_node_p dn = (xml_dump_node_p)n->value;
  xml_dump_info_p xdi = (xml_dump_info_p)in_xdi;
  if (!dn->complete)
    {
    xml_queue_node (xdi, key, dn);
    }
  return 0;
}

/* The xml dump loop.  */
void
xml_dump (xml_dump_info_p xdi)
{
  /* Dump the complete nodes.  */
  while(xdi->queue)
    {
    /* Get the next queue entry.  */
    xml_dump_queue_p dq = xdi->queue;
    tree t = dq->tree_node;
    xml_dump_node_p dn = dq->dump_node;

    /* Remove the entry from the queue.  */
    xdi->queue = dq->next;
    if(!xdi->queue)
      {
      xdi->queue_end = 0;
      }

    /* Put the entry on the free list.  */
    dq->next = xdi->queue_free;
    xdi->queue_free = dq;

    /* Dump the node.  */
    xml_dump_tree_node(xdi, t, dn);
    }
}

/* Queue a filename for later output.  If the file has already been
   queued, it is not queued again.  In either case, the queue index
   assigned to the file is returned.  */
static unsigned int
xml_queue_file (xml_dump_info_p xdi, const char* filename)
{
  tree t = xml_get_encoded_identifier_from_string (filename);

  /* See if we've already queued this file.  */
  splay_tree_node n = splay_tree_lookup (xdi->file_nodes, (splay_tree_key) t);
  if (n)
    {
    /* File was already queued.  Return its index.  */
    return n->value;
    }
  else
    {
    /* File needs to be queued.  */
    xml_file_queue_p fq;

    /* Assign the next available index.  */
    unsigned int index = xdi->file_index++;

    /* Obtain a new queue node.  */
    fq = (xml_file_queue_p) xmalloc (sizeof (struct xml_file_queue));

    /* Create a new entry in the splay-tree.  */
    fq->tree_node = splay_tree_insert (xdi->file_nodes, (splay_tree_key) t,
                                       (splay_tree_value) index);

    /* Add it to the end of the queue.  */
    fq->next = 0;
    if (!xdi->file_queue_end)
      {
      xdi->file_queue = fq;
      }
    else
      {
      xdi->file_queue_end->next = fq;
      }
    xdi->file_queue_end = fq;

    /* Return the index.  */
    return index;
    }
}

/*--------------------------------------------------------------------------*/
/* Print the XML attributes location="fid:line" file="fid" line="line"
   for the given decl.  */
static void
xml_print_location_attribute (xml_dump_info_p xdi, tree d)
{
  unsigned int source_file = xml_queue_file (xdi, DECL_SOURCE_FILE (d));
  unsigned int source_line = DECL_SOURCE_LINE (d);

  fprintf (xdi->file, " location=\"f%d:%d\" file=\"f%d\" line=\"%d\"",
           source_file, source_line, source_file, source_line);
}

static void
xml_document_add_attribute_location(xml_document_element_p element,
                                    xml_document_attribute_use use)
{
  xml_document_add_attribute(element, "location",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_optional, 0);
  xml_document_add_attribute(element, "file",
                             xml_document_attribute_type_string, use, 0);
  xml_document_add_attribute(element, "line",
                             xml_document_attribute_type_integer, use, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute endline="line" for the given STATEMENT_LIST.  */
static void
xml_print_endline_attribute (xml_dump_info_p xdi, tree body)
{
  tree last = EXPR_P (body)? body : 0;
  if (TREE_CODE (body) == STATEMENT_LIST)
    {
    /* Get the last statement in the list.  The list may be empty if the
       body is for an empty implicitly-generated copy-constructor.  */
    tree_stmt_iterator t = tsi_last (body);
    if (!tsi_end_p (t))
      {
      last = tsi_stmt (t);
      }
    }
  if (last && EXPR_HAS_LOCATION (last))
    {
    fprintf (xdi->file, " endline=\"%d\"", EXPR_LINENO (last));
    }
}

static void
xml_document_add_attribute_endline(xml_document_element_p element,
                                   xml_document_attribute_use use)
{
  xml_document_add_attribute(element, "endline",
                             xml_document_attribute_type_integer, use, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute id="..." for the given node.  */
static void
xml_print_id_attribute (xml_dump_info_p xdi, xml_dump_node_p dn)
{
  fprintf (xdi->file, " id=\"_%d\"", dn->index);
}

static void
xml_document_add_attribute_id(xml_document_element_p element)
{
  xml_document_add_attribute(element, "id",
                             xml_document_attribute_type_id,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute name="..." for the given node.  */
static void
xml_print_name_attribute (xml_dump_info_p xdi, tree n)
{
  const char* name = xml_get_encoded_string (n);
  fprintf (xdi->file, " name=\"%s\"", name);
}

static void
xml_document_add_attribute_name(xml_document_element_p element,
                                xml_document_attribute_use use)
{
  xml_document_add_attribute(element, "name",
                             xml_document_attribute_type_string, use, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute mangled="..." for the given node.  */
static void
xml_print_mangled_attribute (xml_dump_info_p xdi, tree n)
{
  if (HAS_DECL_ASSEMBLER_NAME_P(n) &&
      DECL_NAME (n) &&
      DECL_ASSEMBLER_NAME (n) &&
      DECL_ASSEMBLER_NAME (n) != DECL_NAME (n))
    {
    const char* name = xml_get_encoded_string (DECL_ASSEMBLER_NAME (n));
    fprintf (xdi->file, " mangled=\"%s\"", name);
    }
}

static void
xml_document_add_attribute_mangled(xml_document_element_p element)
{
  xml_document_add_attribute(element, "mangled",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute demangled="..." for the given node.  */
static void
xml_print_demangled_attribute (xml_dump_info_p xdi, tree n)
{
  if (HAS_DECL_ASSEMBLER_NAME_P(n) &&
      DECL_NAME (n) &&
      DECL_ASSEMBLER_NAME (n) &&
      DECL_ASSEMBLER_NAME (n) != DECL_NAME (n))
    {
    const char* INTERNAL = " *INTERNAL* ";
    const int demangle_opt =
      (DMGL_STYLE_MASK | DMGL_PARAMS | DMGL_TYPES | DMGL_ANSI) & ~DMGL_JAVA;

    const char* name = xml_get_encoded_string (DECL_ASSEMBLER_NAME (n));
    /*demangled name*/
    char* dename = 0;
    /*duplicated name, used to remove " *INTERNAL* " if found*/
    char* dupl_name = 0;
    /*pointer to found " *INTERNAL* " string*/
    char* internal_found = 0;

    dupl_name = (char*)xmalloc(strlen(name)+1);
    strcpy(dupl_name, name);

    internal_found = strstr( dupl_name, INTERNAL );
    if(internal_found)
      {
      *internal_found = '\0';
      }

    dename = cplus_demangle(dupl_name, demangle_opt);
    if(dename)
      {
      const char* encoded_dename = xml_escape_string(dename);
      fprintf (xdi->file, " demangled=\"%s\"", encoded_dename);
      }
    free(dupl_name);
    }
}

static void
xml_document_add_attribute_demangled(xml_document_element_p element)
{
  xml_document_add_attribute(element, "demangled",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute mutable="..." for the given node.  */
static void
xml_print_mutable_attribute (xml_dump_info_p xdi, tree n)
{
  if (DECL_MUTABLE_P (n))
    {
    fprintf (xdi->file, " mutable=\"1\"");
    }
}

static void
xml_document_add_attribute_mutable(xml_document_element_p element)
{
  xml_document_add_attribute(element, "mutable",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Get the id of the node referenced by the given type after
   accounting for the special CvQualifiedType element.  This is the id
   that should be printed when referencing the type in an IDREF
   attribute.  The integers to which QC, QV, and QR point are set to 1
   or 0 depending on whether the IDREF should include the const,
   volatile, or restrict qualifier.  */
static int
xml_get_idref(xml_dump_info_p xdi, tree t, int complete,
              int* qc, int* qv, int* qr)
{
  /* Get the unqualified node for the type.  */
  int id = xml_add_node (xdi, TYPE_MAIN_VARIANT(t), complete);

  /* Determine the qualifiers that should be added by the IDREF.  */
  *qc = CP_TYPE_CONST_P (t);
  *qv = CP_TYPE_VOLATILE_P (t);
  *qr = CP_TYPE_RESTRICT_P (t);

  if (TYPE_NAME (t))
    {
    /* This type is named by a typedef.  */
    int id2 = xml_add_node (xdi, TYPE_NAME(t), complete);
    if(id2)
      {
      /* The IDREF should reference the typedef.  */
      id = id2;

      /* The IDREF does not need to include cv-qualifiers that
         are contained in the typedef.  */
      if(*qc && CP_TYPE_CONST_P (TREE_TYPE (TYPE_NAME (t))))
        {
        *qc = 0;
        }
      if(*qv && CP_TYPE_VOLATILE_P (TREE_TYPE (TYPE_NAME (t))))
        {
        *qv = 0;
        }
      if(*qr && CP_TYPE_RESTRICT_P (TREE_TYPE (TYPE_NAME (t))))
        {
        *qr = 0;
        }
      }
    }
  return id;
}

/* Print an attribute value referencing the given type.  If the type
   has top-level cv-qualifiers, they are appended to the type's id as
   single characters (c=const, v=volatile, r=restrict), and a
   CvQualifiedType element is generated to create a valid XML
   reference.  */
static void
xml_print_type_idref (xml_dump_info_p xdi, tree t, int complete)
{
  /* Add the unqualified node.  */
  int qc = 0;
  int qv = 0;
  int qr = 0;
  int id = xml_get_idref(xdi, t, complete, &qc, &qv, &qr);

  /* Check cv-qualificiation.  */
  const char* c = qc? "c" : "";
  const char* v = qv? "v" : "";
  const char* r = qr? "r" : "";

  /* If there are any qualifiers, add the qualified node.  */
  if(qc || qv || qr)
    {
    xml_add_node (xdi, t, complete);
    }

  /* Print the reference.  */
  fprintf (xdi->file, "_%d%s%s%s", id, c, v, r);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute type="..." for the given type.  If the type
   has cv-qualifiers, they are appended to the type's id as single
   characters (c=const, v=volatile, r=restrict).  */
static void
xml_print_type_attribute (xml_dump_info_p xdi, tree t, int complete)
{
  fprintf (xdi->file, " type=\"");
  xml_print_type_idref (xdi, t, complete);
  fprintf (xdi->file, "\"");
}

static void
xml_document_add_attribute_type(xml_document_element_p element)
{
  xml_document_add_attribute(element, "type",
                             xml_document_attribute_type_idref,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute returns="tid" for the given function type.  */
static void
xml_print_returns_attribute (xml_dump_info_p xdi, tree t, int complete)
{
  fprintf (xdi->file, " returns=\"");
  xml_print_type_idref (xdi, t, complete);
  fprintf (xdi->file, "\"");
}

static void
xml_document_add_attribute_returns(xml_document_element_p element)
{
  xml_document_add_attribute(element, "returns",
                             xml_document_attribute_type_idref,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute basetype="..." with the given type.  */
static void
xml_print_base_type_attribute (xml_dump_info_p xdi, tree t, int complete)
{
  fprintf (xdi->file, " basetype=\"_%d\"", xml_add_node (xdi, t, complete));
}

static void
xml_document_add_attribute_base_type(xml_document_element_p element)
{
  xml_document_add_attribute(element, "basetype",
                             xml_document_attribute_type_idref,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute context="..." for the given node.  If the
   context is a type also print the XML attribute access="..." for the
   given decl.  */
static void
xml_print_context_attribute (xml_dump_info_p xdi, tree n)
{
  if(n != global_namespace)
    {
    tree context = CP_DECL_CONTEXT (n);
    if(context)
      {
      /* Print the context attribute.  */
      fprintf (xdi->file, " context=\"_%d\"",
               xml_add_node (xdi, context, 0));

      /* If the context is a type, print the access attribute.  */
      if (TYPE_P(context))
        {
        if (TREE_PRIVATE (n))
          {
          fprintf (xdi->file, " access=\"private\"");
          }
        else if (TREE_PROTECTED (n))
          {
          fprintf (xdi->file, " access=\"protected\"");
          }
        else
          {
          fprintf (xdi->file, " access=\"public\"");
          }
        }
      }
    }
}

static void
xml_document_add_attribute_access(xml_document_element_p element)
{
  xml_document_add_attribute(element, "access",
                             xml_document_attribute_type_enum_access,
                             xml_document_attribute_use_optional, "public");
}

static void
xml_document_add_attribute_context(xml_document_element_p element,
                                   xml_document_attribute_use use,
                                   int do_access)
{
  xml_document_add_attribute(element, "context",
                             xml_document_attribute_type_idref, use, 0);
  if(do_access)
    {
    xml_document_add_attribute_access(element);
    }
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute explicit="..." for the given decl.  */
static void
xml_print_explicit_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_NONCONVERTING_P (d))
    {
    fprintf (xdi->file, " explicit=\"1\"");
    }
}

static void
xml_document_add_attribute_explicit(xml_document_element_p element)
{
  xml_document_add_attribute(element, "explicit",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_optional, "1");
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute size="..." for the given type.  */
static void
xml_print_size_attribute (xml_dump_info_p xdi, tree t)
{
  tree size_tree = TYPE_SIZE (t);
  if (size_tree && host_integerp (size_tree, 1))
    {
    unsigned HOST_WIDE_INT size = tree_low_cst (size_tree, 1);
    fprintf (xdi->file, " size=\"" HOST_WIDE_INT_PRINT_UNSIGNED "\"", size);
    }
}

static void
xml_document_add_attribute_size(xml_document_element_p element)
{
  xml_document_add_attribute(element, "size",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute align="..." for the given type.  */
static void
xml_print_align_attribute (xml_dump_info_p xdi, tree t)
{
  fprintf (xdi->file, " align=\"%d\"", TYPE_ALIGN (t));
}

static void
xml_document_add_attribute_align(xml_document_element_p element)
{
  xml_document_add_attribute(element, "align",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute offset="..." for the given decl.  */
static void
xml_print_offset_attribute (xml_dump_info_p xdi, tree d)
{
  tree tree_byte_ofs = DECL_FIELD_OFFSET(d);
  tree tree_bit_ofs = DECL_FIELD_BIT_OFFSET(d);
  if (tree_byte_ofs && host_integerp(tree_byte_ofs, 1) &&
      tree_bit_ofs && host_integerp(tree_bit_ofs, 1))
    {
    unsigned HOST_WIDE_INT bit_ofs = tree_low_cst (tree_bit_ofs, 1);
    unsigned HOST_WIDE_INT byte_ofs = tree_low_cst (tree_byte_ofs, 1);
    fprintf(xdi->file, " offset=\"" HOST_WIDE_INT_PRINT_UNSIGNED "\"",
            byte_ofs * 8 + bit_ofs);
    }
}

static void
xml_document_add_attribute_offset(xml_document_element_p element)
{
  xml_document_add_attribute(element, "offset",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute const="1" for const methods.  */
static void
xml_print_const_method_attribute (xml_dump_info_p xdi, tree fd)
{
  if (DECL_CONST_MEMFUNC_P (fd))
    {
    fprintf (xdi->file, " const=\"1\"");
    }
}

static void
xml_document_add_attribute_const_method(xml_document_element_p element)
{
  xml_document_add_attribute(element, "const",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute static="1" for static methods.  */
static void
xml_print_static_method_attribute (xml_dump_info_p xdi, tree fd)
{
  if (!DECL_NONSTATIC_MEMBER_FUNCTION_P (fd))
    {
    fprintf (xdi->file, " static=\"1\"");
    }
}

static void
xml_document_add_attribute_static(xml_document_element_p element)
{
  xml_document_add_attribute(element, "static",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute static="1" for static functions and variables.  */
static void
xml_print_static_attribute (xml_dump_info_p xdi, tree fd)
{
  if (DECL_THIS_STATIC (fd))
    {
    fprintf (xdi->file, " static=\"1\"");
    }
}

/*--------------------------------------------------------------------------*/
static int
xml_print_overrides_r (xml_dump_info_p xdi, tree type, tree fndecl);

static int
xml_print_overrides (xml_dump_info_p xdi, tree type, tree fndecl)
{
  tree binfo = TYPE_BINFO (type);
  tree base_binfo;
  int ix;
  int found = 0;

  for (ix = 0; BINFO_BASE_ITERATE (binfo, ix, base_binfo); ix++)
    {
    tree basetype = BINFO_TYPE (base_binfo);

    if (TYPE_POLYMORPHIC_P (basetype))
      {
      found += xml_print_overrides_r (xdi, basetype, fndecl);
      }
    }
  return found;
}

/* Look in TYPE for virtual functions overrides by FNDECL.  Check both
   TYPE itself and its bases.  */
static int
xml_print_overrides_r (xml_dump_info_p xdi, tree type, tree fndecl)
{
  tree fn = look_for_overrides_here (type, fndecl);

  if (fn)
    {
    int id = xml_add_node (xdi, fn, 1);

    if(id)
      {
      fprintf (xdi->file, "_%d ", id);
      }

    return 1;
    }

  /* We failed to find one declared in this class. Look in its bases.  */
  return xml_print_overrides (xdi, type, fndecl);
}

static void
xml_print_overrides_method_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_VIRTUAL_P (d))
    {
    fprintf (xdi->file, " overrides=\"");
    xml_print_overrides(xdi, CP_DECL_CONTEXT(d), d);
    fprintf (xdi->file, "\"");
    }
}

static void
xml_document_add_attribute_overrides_method(xml_document_element_p element)
{
  xml_document_add_attribute(element, "overrides",
                             xml_document_attribute_type_idrefs,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML attributes virtual="", overrides="", and pure_virtual=""
   for a decl.  */
static void
xml_print_virtual_method_attributes (xml_dump_info_p xdi, tree d)
{
  if (DECL_VIRTUAL_P (d))
    {
    fprintf (xdi->file, " virtual=\"1\"");
    }

  if (DECL_PURE_VIRTUAL_P (d))
    {
    fprintf (xdi->file, " pure_virtual=\"1\"");
    }

  xml_print_overrides_method_attribute(xdi, d);
}

static void
xml_document_add_attribute_virtual_method(xml_document_element_p element)
{
  xml_document_add_attribute(element, "virtual",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_attribute(element, "pure_virtual",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_attribute_overrides_method(element);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute extern="1" if the given decl is external.  */
static void
xml_print_extern_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_EXTERNAL (d))
    {
    fprintf (xdi->file, " extern=\"1\"");
    }
}

static void
xml_document_add_attribute_extern(xml_document_element_p element)
{
  xml_document_add_attribute(element, "extern",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute inline="1" if the given decl is inline.  */
static void
xml_print_inline_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_DECLARED_INLINE_P (d))
    {
    fprintf (xdi->file, " inline=\"1\"");
    }
}

static void
xml_document_add_attribute_inline(xml_document_element_p element)
{
  xml_document_add_attribute(element, "inline",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute extern="1" if the given decl is external.  */
static void
xml_print_function_extern_attribute (xml_dump_info_p xdi, tree fd)
{
  if (DECL_REALLY_EXTERN (fd))
    {
    fprintf (xdi->file, " extern=\"1\"");
    }
}

static void
xml_document_add_attribute_function_extern(xml_document_element_p element)
{
  xml_document_add_attribute(element, "extern",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute for a default argument.  */
static void
xml_print_default_argument_attribute (xml_dump_info_p xdi, tree t)
{
  const char* value;
  value = xml_get_encoded_string_from_string (expr_as_string (t, 0));
  fprintf (xdi->file, " default=\"%s\"", value);
}

static void
xml_document_add_attribute_default_argument(xml_document_element_p element,
                                            xml_document_attribute_use use)
{
  xml_document_add_attribute(element, "default",
                             xml_document_attribute_type_string, use, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute init="..." for a variable initializer.  */
static void
xml_print_init_attribute (xml_dump_info_p xdi, tree t)
{
  const char* value;

  if (!t || (t == error_mark_node)) return;

  value = xml_get_encoded_string_from_string (expr_as_string (t, 0));
  fprintf (xdi->file, " init=\"%s\"", value);
}

static void
xml_document_add_attribute_init(xml_document_element_p element)
{
  xml_document_add_attribute(element, "init",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute incomplete="..." for the given type.  */
static void
xml_print_incomplete_attribute (xml_dump_info_p xdi, tree t)
{
  if (!COMPLETE_TYPE_P (t))
    {
    fprintf (xdi->file, " incomplete=\"1\"");
    }
}

static void
xml_document_add_attribute_incomplete(xml_document_element_p element)
{
  xml_document_add_attribute(element, "init",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute abstract="..." for the given type.  */
static void
xml_print_abstract_attribute (xml_dump_info_p xdi, tree t)
{
  if (CLASSTYPE_PURE_VIRTUALS (t) != 0)
    {
    fprintf (xdi->file, " abstract=\"1\"");
    }
}

static void
xml_document_add_attribute_abstract(xml_document_element_p element)
{
  xml_document_add_attribute(element, "abstract",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print XML empty tag for an ellipsis at the end of an argument list.  */
static void
xml_output_ellipsis (xml_dump_info_p xdi)
{
  fprintf (xdi->file,
           "    <Ellipsis/>\n");
}

static void
xml_document_add_element_ellipsis (xml_document_info_p xdi,
                                   xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 0);
  e->name = "Ellipsis";
}

/*--------------------------------------------------------------------------*/
/* Print XML attributes min="0" max="..." for an array type.  */
static void
xml_print_array_attributes (xml_dump_info_p xdi, tree at)
{
  const char* length = "";

  if (TYPE_DOMAIN (at))
    length = xml_get_encoded_string_from_string (
      expr_as_string (TYPE_MAX_VALUE (TYPE_DOMAIN (at)), 0));

  fprintf (xdi->file, " min=\"0\" max=\"%s\"", length);
}

static void
xml_document_add_attribute_array(xml_document_element_p element)
{
  xml_document_add_attribute(element, "min",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_required, "0");
  xml_document_add_attribute(element, "max",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_required, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute listing types that this type can throw.  */
static void
xml_print_throw_attribute (xml_dump_info_p xdi, tree ft, int complete)
{
  tree raises = TYPE_RAISES_EXCEPTIONS (ft);
  if(raises)
    {
    fprintf (xdi->file, " throw=\"");
    if(TREE_VALUE (raises))
      {
      for (;
           raises != NULL_TREE; raises = TREE_CHAIN (raises))
        {
        fprintf (xdi->file, "%s",
                 (raises == TYPE_RAISES_EXCEPTIONS (ft))?"":" ");
        xml_print_type_idref (xdi, TREE_VALUE (raises), complete);
        }
      }
    fprintf (xdi->file, "\"");
    }
}

static void
xml_document_add_attribute_throw(xml_document_element_p element)
{
  xml_document_add_attribute(element, "throw",
                             xml_document_attribute_type_idrefs,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Given an attribute node, set "arg" to the string value of the first
   argument, and return the first argument node.  */
tree xml_get_first_attrib_arg(tree attrib_node, char** arg)
{
  /* This function contributed by Steven Kilthau - May 2004.  */
  tree arg_node = TREE_VALUE (attrib_node);
  *arg = "";
  if (arg_node && (TREE_CODE (arg_node) == TREE_LIST))
    {
    tree cst = TREE_VALUE (arg_node);
    if (TREE_CODE (cst) == STRING_CST)
      {
      *arg = TREE_STRING_POINTER (cst);
      }
    return arg_node;
    }
  return 0;
}

/* Given an argument node, set "arg" to the string value of the next
   argument, and return the next argument node.  */
tree xml_get_next_attrib_arg(tree arg_node, char** arg)
{
  /* This function contributed by Steven Kilthau - May 2004.  */
  arg_node = TREE_CHAIN (arg_node);
  *arg = "";
  if (arg_node && (TREE_CODE (arg_node) == TREE_LIST))
    {
    tree cst = TREE_VALUE (arg_node);
    if (TREE_CODE (cst) == STRING_CST)
      {
      *arg = TREE_STRING_POINTER (cst);
      }
    return arg_node;
    }
  return 0;
}

/* Print XML attribute listing the contents of the __attribute__ node
   given.  */
static void
xml_print_attributes_attribute (xml_dump_info_p xdi, tree attributes1,
                                tree attributes2)
{
  if(attributes1 || attributes2)
    {
    const char* space = "";
    tree attribute;
    tree arg_node;
    char* arg;
    fprintf(xdi->file, " attributes=\"");
    for(attribute = attributes1; attribute;
        attribute = TREE_CHAIN(attribute))
      {
      fprintf(xdi->file, "%s%s", space,
              xml_get_encoded_string(TREE_PURPOSE (attribute)));
      space = " ";

      /* Format and print the string arguments to the attribute
         (contributed by Steven Kilthau - May 2004).  */
      if ((arg_node = xml_get_first_attrib_arg(attribute, &arg)) != 0)
        {
        fprintf(xdi->file, "(%s", xml_get_encoded_string_from_string(arg));
        while((arg_node = xml_get_next_attrib_arg(arg_node, &arg)) != 0)
          {
          fprintf(xdi->file, ",%s", xml_get_encoded_string_from_string(arg));
          }
        fprintf(xdi->file, ")");
        }
      }
    for(attribute = attributes2; attribute;
        attribute = TREE_CHAIN(attribute))
      {
      fprintf(xdi->file, "%s%s", space,
              xml_get_encoded_string(TREE_PURPOSE (attribute)));
      space = " ";

      /* Format and print the string arguments to the attribute
         (contributed by Steven Kilthau - May 2004).  */
      if ((arg_node = xml_get_first_attrib_arg(attribute, &arg)) != 0)
        {
        fprintf(xdi->file, "(%s", xml_get_encoded_string_from_string(arg));
        while((arg_node = xml_get_next_attrib_arg(arg_node, &arg)) != 0)
          {
          fprintf(xdi->file, ",%s", xml_get_encoded_string_from_string(arg));
          }
        fprintf(xdi->file, ")");
        }
      }
    fprintf(xdi->file, "\"");
    }
}

static void
xml_document_add_attribute_attributes(xml_document_element_p element)
{
  xml_document_add_attribute(element, "attributes",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML attribute artificial="1" for compiler generated methods.  */
static void
xml_print_artificial_attribute(xml_dump_info_p xdi, tree d)
{
  if (DECL_ARTIFICIAL (d))
    {
    fprintf (xdi->file, " artificial=\"1\"");
    }
}

static void
xml_document_add_attribute_artificial(xml_document_element_p element)
{
  xml_document_add_attribute(element, "artificial",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute bits="..." for a bitfield.  */
static void
xml_print_bits_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_BIT_FIELD_TYPE (d))
    {
    tree size_tree = DECL_SIZE (d);

    if (size_tree && host_integerp (size_tree, 1))
      {
      unsigned HOST_WIDE_INT bits = tree_low_cst(size_tree, 1);
      fprintf (xdi->file, " bits=\"" HOST_WIDE_INT_PRINT_UNSIGNED "\"", bits);
      }
    }
}

static void
xml_document_add_attribute_bits(xml_document_element_p element)
{
  xml_document_add_attribute(element, "bits",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print the XML attribute befriending="..." for a function or class.  */
static void
xml_print_befriending_attribute (xml_dump_info_p xdi, tree befriending)
{
  int have_befriending = 0;
  tree frnd;
  for (frnd = befriending ; frnd && !have_befriending ;
       frnd = TREE_CHAIN (frnd))
    {
    if(TREE_CODE (TREE_VALUE (frnd)) != TEMPLATE_DECL)
      {
      have_befriending = 1;
      }
    }
  if(have_befriending)
    {
    const char* sep = "";
    fprintf (xdi->file, " befriending=\"");
    for (frnd = befriending ; frnd ; frnd = TREE_CHAIN (frnd))
      {
      if(TREE_CODE (TREE_VALUE (frnd)) != TEMPLATE_DECL)
        {
        fprintf (xdi->file,
                 "%s_%d", sep, xml_add_node (xdi, TREE_VALUE (frnd), 0));
        sep = " ";
        }
      }
    fprintf (xdi->file, "\"");
    }
}

static void
xml_document_add_attribute_befriending(xml_document_element_p element)
{
  xml_document_add_attribute(element, "befriending",
                             xml_document_attribute_type_idrefs,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/
/* Print XML empty tag describing an unimplemented TREE_CODE that has been
   encountered.  */
static void
xml_output_unimplemented (xml_dump_info_p xdi, tree t, xml_dump_node_p dn,
                          const char* where)
{
  int tree_code = TREE_CODE (t);
  fprintf (xdi->file, "  <Unimplemented");
  if(dn)
    {
    fprintf (xdi->file, " id=\"_%d\"", dn->index);
    }
  fprintf (xdi->file, " tree_code=\"%d\" tree_code_name=\"%s\" node=\"%p\"",
           tree_code, tree_code_name [tree_code], t);
  if (where)
    {
    fprintf (xdi->file, " function=\"%s\"", where);
    }
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_unimplemented (xml_document_info_p xdi,
                                        xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Unimplemented";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute(e, "tree_code",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_required, 0);
  xml_document_add_attribute(e, "tree_code_name",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_required, 0);
  xml_document_add_attribute(e, "node",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_required, 0);
  xml_document_add_attribute(e, "function",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_optional, 0);
}

/*--------------------------------------------------------------------------*/

/* Dump a NAMESPACE_DECL.  */
static void
xml_output_namespace_decl (xml_dump_info_p xdi, tree ns, xml_dump_node_p dn)
{
  /* Only walk a real namespace.  */
  if (!DECL_NAMESPACE_ALIAS (ns))
    {
    fprintf (xdi->file, "  <Namespace");
    xml_print_id_attribute (xdi, dn);
    if(DECL_NAME (ns) != NULL_TREE) /* anonymous_namespace_name */
      {
      xml_print_name_attribute (xdi, DECL_NAME (ns));
      }
    xml_print_context_attribute (xdi, ns);
    xml_print_attributes_attribute (xdi, DECL_ATTRIBUTES(ns), 0);

    /* If complete dump, walk the namespace.  */
    if(dn->complete)
      {
      /* Get the vector of all declarations in the namespace.  */
      int i;
      VEC(tree,gc) *decls = NAMESPACE_LEVEL (ns)->all_decls;
      tree *vec = VEC_address (tree, decls);
      int len = VEC_length (tree, decls);

      /* Output all the declarations.  */
      fprintf (xdi->file, " members=\"");
      for (i=0; i < len; ++i)
        {
        int id = xml_add_node (xdi, vec[i], 1);
        if (id)
          {
          fprintf (xdi->file, "_%d ", id);
          }
        }
      fprintf (xdi->file, "\"");
      }

    xml_print_mangled_attribute (xdi, ns);
    xml_print_demangled_attribute (xdi, ns);
    fprintf (xdi->file, "/>\n");
    }
  /* If it is a namespace alias, just indicate that.  */
  else
    {
    tree real_ns = ns;
    /* Find the real namespace.  */
    while (DECL_NAMESPACE_ALIAS (real_ns))
      {
      real_ns = DECL_NAMESPACE_ALIAS (real_ns);
      }

    fprintf (xdi->file, "  <NamespaceAlias");
    xml_print_id_attribute (xdi, dn);
    xml_print_name_attribute (xdi, DECL_NAME (ns));
    xml_print_context_attribute (xdi, ns);
    fprintf (xdi->file, " namespace=\"_%d\"",
             xml_add_node (xdi, real_ns, 0));
    xml_print_mangled_attribute (xdi, ns);
    xml_print_demangled_attribute (xdi, ns );
    fprintf (xdi->file, "/>\n");
    }
}

static void
xml_document_add_element_namespace_decl (xml_document_info_p xdi,
                                         xml_document_element_p parent)
{
  {
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Namespace";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     0);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute(e, "members",
                             xml_document_attribute_type_idrefs,
                             xml_document_attribute_use_optional, 0);
  xml_document_add_attribute_mangled(e);
  xml_document_add_attribute_demangled(e);
  }
  {
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "NamespaceAlias";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     0);
  xml_document_add_attribute(e, "namespace",
                             xml_document_attribute_type_idref,
                             xml_document_attribute_use_required, 0);
  xml_document_add_attribute_mangled(e);
  xml_document_add_attribute_demangled(e);
  }
}

/*--------------------------------------------------------------------------*/
/* Output for a typedef.  The name and associated type are output.  */
static void
xml_output_typedef (xml_dump_info_p xdi, tree td, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <Typedef");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (td));

  /* Get the original type out of the typedef, if any.  */
  if (DECL_ORIGINAL_TYPE (td))
    {
    xml_print_type_attribute (xdi, DECL_ORIGINAL_TYPE (td), dn->complete);
    }
  else
    {
    xml_print_type_attribute (xdi, TREE_TYPE (td), dn->complete);
    }

  xml_print_context_attribute (xdi, td);
  xml_print_location_attribute (xdi, td);

  /* Output typedef attributes (contributed by Steven Kilthau - May 2004).  */
  if (td)
    {
    xml_print_attributes_attribute (xdi, DECL_ATTRIBUTES(td), 0);
    }

  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_typedef (xml_document_info_p xdi,
                                  xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Typedef";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     1);
  xml_document_add_attribute_location(e, xml_document_attribute_use_required);
  xml_document_add_attribute_attributes(e);
}

/*--------------------------------------------------------------------------*/
/* Output for a PARM_DECL / TREE_LIST corresponding to a function argument.  */
static void
xml_output_argument (xml_dump_info_p xdi, tree pd, tree tl, int complete)
{
  /* Don't process any compiler-generated arguments.  These occur for
     things like constructors of classes with virtual inheritance.  */
  if (pd && DECL_ARTIFICIAL (pd)) return;

  fprintf (xdi->file, "    <Argument");

  if (pd && DECL_NAME (pd))
    {
    xml_print_name_attribute (xdi, DECL_NAME (pd));
    }
  if (pd && TREE_TYPE (pd))
    {
    tree t = TREE_TYPE(pd);
    if (DECL_BY_REFERENCE (pd))
      {
      /* This is an "invisible reference" added by GCC.  Replace it
         with the real type.  */
      t = TREE_TYPE (t);
      }
    xml_print_type_attribute (xdi, t, complete);
    xml_print_location_attribute (xdi, pd);
    }
  else
    {
    xml_print_type_attribute (xdi, TREE_VALUE (tl), complete);
    }

  if (TREE_PURPOSE (tl))
    {
    xml_print_default_argument_attribute (xdi, TREE_PURPOSE (tl));
    }

  /* Output argument attributes (contributed by Steven Kilthau - May 2004).  */
  if (pd)
    {
    xml_print_attributes_attribute (xdi, DECL_ATTRIBUTES(pd), 0);
    }

  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_argument (xml_document_info_p xdi,
                                   xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Argument";
  xml_document_add_attribute_name(e, xml_document_attribute_use_optional);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_location(e, xml_document_attribute_use_optional);
  xml_document_add_attribute_default_argument(
    e, xml_document_attribute_use_optional);
  xml_document_add_attribute_attributes(e);
}

/*--------------------------------------------------------------------------*/
/* Lookup the real name of an operator whose ansi_opname or ansi_assopname
   is NAME.  */
static tree
xml_reverse_opname_lookup (tree name)
{
  static const char* unknown_operator = "{unknown operator}";
  int i;

  /* Make sure we know about this internal name.  */
  if (!IDENTIFIER_OPNAME_P (name))
    return get_identifier (unknown_operator);

  /* Search the list of internal anmes */
  for (i=0; i < LAST_CPLUS_TREE_CODE ; ++i)
    {
    if (ansi_opname(i) == name)
      return get_identifier (operator_name_info[i].name);
    else if (ansi_assopname(i) == name)
      return get_identifier (assignment_operator_name_info[i].name);
    }

  return get_identifier (unknown_operator);
}

/* Output for a FUNCTION_DECL.  */
static void
xml_output_function_decl (xml_dump_info_p xdi, tree fd, xml_dump_node_p dn)
{
  tree arg;
  tree arg_type;
  const char* tag;
  tree name = DECL_NAME (fd);
  tree saved_tree = DECL_SAVED_TREE (fd);
  tree body = saved_tree? BIND_EXPR_BODY (saved_tree) : 0;
  int do_name = 1;
  int do_returns = 0;
  int do_const = 0;
  int do_virtual = 0;
  int do_static_method = 0;
  int do_static_function = 0;
  int do_artificial = 0;
  int do_explicit = 0;

  /* Print out the begin tag for this type of function.  */
  if (DECL_CONSTRUCTOR_P (fd))
    {
    /* A class constructor.  */
    tag = "Constructor"; do_artificial = 1;
    do_explicit = 1;
    if(TYPE_ANONYMOUS_P (DECL_CONTEXT (fd)))
      {
      do_name = 0;
      }
    }
  else if (DECL_DESTRUCTOR_P (fd))
    {
    /* A class destructor.  */
    tag = "Destructor"; do_virtual = 1; do_artificial = 1;
    }
  else if (DECL_OVERLOADED_OPERATOR_P (fd))
    {
    if (DECL_CONV_FN_P (fd))
      {
      /* A type-conversion operator in a class.  */
      tag = "Converter";
      do_returns = 1; do_const = 1; do_virtual = 1;
      }
    else
      {
      if (DECL_FUNCTION_MEMBER_P (fd))
        {
        /* An operator in a class.  */
        tag = "OperatorMethod";
        name = xml_reverse_opname_lookup (DECL_NAME (fd));
        do_returns = 1; do_const = 1; do_virtual = 1;
        do_static_method = 1; do_artificial = 1;
        }
      else
        {
        /* An operator in a namespace.  */
        tag = "OperatorFunction";
        name = xml_reverse_opname_lookup (DECL_NAME (fd));
        do_static_function= 1; do_returns = 1;
        }
      }
    }
  else
    {
    if (DECL_FUNCTION_MEMBER_P (fd))
      {
      /* A member of a class.  */
      tag = "Method"; do_returns = 1; do_const = 1;
      do_virtual = 1; do_static_method = 1;
      }
    else
      {
      /* A member of a namespace.  */
      tag = "Function"; do_returns = 1; do_static_function = 1;
      }
    }

  fprintf (xdi->file, "  <%s", tag);
  xml_print_id_attribute (xdi, dn);
  if(do_name)
    {
    xml_print_name_attribute (xdi, name);
    }

  if(do_returns)
    {
    xml_print_returns_attribute (xdi, TREE_TYPE (TREE_TYPE (fd)), dn->complete);
    }
  if(do_explicit) xml_print_explicit_attribute (xdi, fd);
  if(do_const)   xml_print_const_method_attribute (xdi, fd);
  if(do_virtual) xml_print_virtual_method_attributes (xdi, fd);
  if(do_static_method)  xml_print_static_method_attribute (xdi, fd);
  if(do_static_function)  xml_print_static_attribute (xdi, fd);
  if(do_artificial)  xml_print_artificial_attribute (xdi, fd);
  xml_print_throw_attribute (xdi, TREE_TYPE (fd), dn->complete);
  xml_print_context_attribute (xdi, fd);
  xml_print_mangled_attribute (xdi, fd);
  xml_print_demangled_attribute (xdi, fd);
  xml_print_location_attribute (xdi, fd);
  if(body)
    {
    xml_print_endline_attribute (xdi, body);
    }
  xml_print_function_extern_attribute (xdi, fd);
  xml_print_inline_attribute (xdi, fd);
  xml_print_attributes_attribute (xdi, DECL_ATTRIBUTES(fd),
                                  TYPE_ATTRIBUTES(TREE_TYPE(fd)));
  xml_print_befriending_attribute (xdi, DECL_BEFRIENDING_CLASSES (fd));

  /* Prepare to iterator through argument list.  */
  arg = DECL_ARGUMENTS (fd);
  arg_type = TYPE_ARG_TYPES (TREE_TYPE (fd));
  if (DECL_NONSTATIC_MEMBER_FUNCTION_P (fd))
    {
    /* Skip "this" argument.  */
    if(arg) arg = TREE_CHAIN (arg);
    arg_type = TREE_CHAIN (arg_type);
    }

  /* If there are no arguments, finish the element.  */
  if (arg_type == void_list_node)
    {
    fprintf (xdi->file, "/>\n");
    return;
    }
  else
    {
    fprintf (xdi->file, ">\n");
    }

  /* Print out the argument list for this function.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (xdi, arg, arg_type, dn->complete);
    if(arg) arg = TREE_CHAIN (arg);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(!arg_type)
    {
    /* Function has variable number of arguments.  Print ellipsis.  */
    xml_output_ellipsis (xdi);
    }

  fprintf (xdi->file, "  </%s>\n", tag);
}

static void
xml_document_add_element_function_helper (xml_document_info_p xdi,
                                          xml_document_element_p parent,
                                          const char* tag,
                                          int do_returns,
                                          int do_access,
                                          int do_const,
                                          int do_virtual,
                                          int do_static,
                                          int do_artificial,
                                          int do_explicit,
                                          int allow_arguments,
                                          int allow_ellipsis)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = tag;
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  if(do_returns)
    {
    xml_document_add_attribute_returns(e);
    }
  if(do_const)
    {
    xml_document_add_attribute_const_method(e);
    }
  if(do_virtual)
    {
    xml_document_add_attribute_virtual_method(e);
    }
  if(do_static)
    {
    xml_document_add_attribute_static(e);
    }
  if(do_artificial)
    {
    xml_document_add_attribute_artificial(e);
    }
  if(do_explicit)
    {
    xml_document_add_attribute_explicit(e);
    }
  xml_document_add_attribute_throw(e);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     do_access);
  xml_document_add_attribute_mangled(e);
  xml_document_add_attribute_demangled(e);
  xml_document_add_attribute_location(e, xml_document_attribute_use_required);
  xml_document_add_attribute_endline(e, xml_document_attribute_use_optional);
  xml_document_add_attribute_extern(e);
  xml_document_add_attribute_inline(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_befriending(e);
  if(allow_arguments)
    {
    xml_document_add_element_argument (xdi, e);
    }
  if(allow_ellipsis)
    {
    xml_document_add_element_ellipsis (xdi, e);
    }
}

static void
xml_document_add_element_function (xml_document_info_p xdi,
                                   xml_document_element_p parent)
{
  xml_document_add_element_function_helper(
    xdi, parent, "Constructor", /*do_returns*/ 0, /*do_access*/ 1,
    /*do_const*/ 0, /*do_virtual*/ 0, /*do_static*/ 0, /*do_artificial*/ 1,
    /*do_explicit*/ 1, /*allow_arguments*/ 1, /*allow_ellipsis*/ 1);
  xml_document_add_element_function_helper(
    xdi, parent, "Destructor", /*do_returns*/ 0, /*do_access*/ 1,
    /*do_const*/ 0, /*do_virtual*/ 1, /*do_static*/ 0, /*do_artificial*/ 1,
    /*do_explicit*/ 0, /*allow_arguments*/ 0, /*allow_ellipsis*/ 0);
  xml_document_add_element_function_helper(
    xdi, parent, "Converter", /*do_returns*/ 1, /*do_access*/ 1,
    /*do_const*/ 1, /*do_virtual*/ 1, /*do_static*/ 0, /*do_artificial*/ 0,
    /*do_explicit*/ 0, /*allow_arguments*/ 0, /*allow_ellipsis*/ 0);
  xml_document_add_element_function_helper(
    xdi, parent, "OperatorMethod", /*do_returns*/ 1, /*do_access*/ 1,
    /*do_const*/ 1, /*do_virtual*/ 1, /*do_static*/ 1, /*do_artificial*/ 0,
    /*do_explicit*/ 0, /*allow_arguments*/ 1, /*allow_ellipsis*/ 0);
  xml_document_add_element_function_helper(
    xdi, parent, "OperatorFunction", /*do_returns*/ 1, /*do_access*/ 0,
    /*do_const*/ 0, /*do_virtual*/ 0, /*do_static*/ 1, /*do_artificial*/ 0,
    /*do_explicit*/ 0, /*allow_arguments*/ 1, /*allow_ellipsis*/ 0);
  xml_document_add_element_function_helper(
    xdi, parent, "Method", /*do_returns*/ 1, /*do_access*/ 1,
    /*do_const*/ 1, /*do_virtual*/ 1, /*do_static*/ 1, /*do_artificial*/ 0,
    /*do_explicit*/ 0, /*allow_arguments*/ 1, /*allow_ellipsis*/ 1);
  xml_document_add_element_function_helper(
    xdi, parent, "Function", /*do_returns*/ 1, /*do_access*/ 0,
    /*do_const*/ 0, /*do_virtual*/ 0, /*do_static*/ 1, /*do_artificial*/ 0,
    /*do_explicit*/ 0, /*allow_arguments*/ 1, /*allow_ellipsis*/ 1);
}

/*--------------------------------------------------------------------------*/
/* Output for a VAR_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
static void
xml_output_var_decl (xml_dump_info_p xdi, tree vd, xml_dump_node_p dn)
{
  tree type = TREE_TYPE (vd);
  fprintf (xdi->file, "  <Variable");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (vd));
  xml_print_type_attribute (xdi, type, dn->complete);
  xml_print_init_attribute (xdi, DECL_INITIAL (vd));
  xml_print_context_attribute (xdi, vd);
  xml_print_mangled_attribute (xdi, vd);
  xml_print_demangled_attribute (xdi, vd );
  xml_print_location_attribute (xdi, vd);
  xml_print_static_attribute(xdi, vd);
  xml_print_extern_attribute (xdi, vd);
  xml_print_artificial_attribute (xdi, vd);
  xml_print_attributes_attribute (xdi, DECL_ATTRIBUTES(vd), 0);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_var_decl (xml_document_info_p xdi,
                                   xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Variable";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_init(e);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     1);
  xml_document_add_attribute_mangled(e);
  xml_document_add_attribute_demangled(e);
  xml_document_add_attribute_location(e, xml_document_attribute_use_required);
  xml_document_add_attribute_static(e);
  xml_document_add_attribute_extern(e);
  xml_document_add_attribute_artificial(e);
  xml_document_add_attribute_attributes(e);
}

/*--------------------------------------------------------------------------*/
/* Output for a FIELD_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
static void
xml_output_field_decl (xml_dump_info_p xdi, tree fd, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <Field");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (fd));
  xml_print_bits_attribute(xdi, fd);
  if (DECL_BIT_FIELD_TYPE (fd))
    {
    xml_print_type_attribute (xdi, DECL_BIT_FIELD_TYPE (fd), dn->complete);
    }
  else
    {
    xml_print_type_attribute (xdi, TREE_TYPE (fd), dn->complete);
    }
  xml_print_offset_attribute (xdi, fd);
  xml_print_context_attribute (xdi, fd);
  xml_print_mangled_attribute (xdi, fd);
  xml_print_demangled_attribute (xdi, fd);
  xml_print_mutable_attribute(xdi, fd);
  xml_print_location_attribute (xdi, fd);
  xml_print_attributes_attribute (xdi, DECL_ATTRIBUTES(fd), 0);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_field_decl (xml_document_info_p xdi,
                                     xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Field";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_bits(e);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_offset(e);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     1);
  xml_document_add_attribute_mangled(e);
  xml_document_add_attribute_demangled(e);
  xml_document_add_attribute_mutable(e);
  xml_document_add_attribute_location(e, xml_document_attribute_use_required);
  xml_document_add_attribute_attributes(e);
}

/*--------------------------------------------------------------------------*/
/* Output a RECORD_TYPE that is not a pointer-to-member-function.
   Prints beginning and ending tags, and all class member declarations
   between.  Also handles a UNION_TYPE.  */
static void
xml_output_record_type (xml_dump_info_p xdi, tree rt, xml_dump_node_p dn)
{
  int has_bases = 0;
  tree field;
  tree func;
  const char* tag;

  if (TREE_CODE(rt) == RECORD_TYPE)
    {
    if (CLASSTYPE_DECLARED_CLASS (rt)) { tag = "Class"; }
    else { tag = "Struct"; }
    }
  else { tag = "Union"; }

  fprintf (xdi->file, "  <%s", tag);
  xml_print_id_attribute (xdi, dn);
  if(!TYPE_ANONYMOUS_P (rt))
    {
    xml_print_name_attribute (xdi, DECL_NAME (TYPE_NAME (rt)));
    }
  xml_print_context_attribute (xdi, TYPE_NAME (rt));
  xml_print_abstract_attribute (xdi, rt);
  xml_print_incomplete_attribute (xdi, rt);
  xml_print_mangled_attribute (xdi, TYPE_NAME (rt));
  xml_print_demangled_attribute (xdi, TYPE_NAME (rt));
  xml_print_location_attribute (xdi, TYPE_NAME (rt));
  xml_print_artificial_attribute (xdi, TYPE_NAME (rt));
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(rt), 0);
  xml_print_size_attribute (xdi, rt);
  xml_print_align_attribute (xdi, rt);
  xml_print_befriending_attribute (xdi, CLASSTYPE_BEFRIENDING_CLASSES (rt));

  if (dn->complete && COMPLETE_TYPE_P (rt))
    {
    fprintf (xdi->file, " members=\"");
    /* Output all the non-method declarations in the class.  */
    for (field = TYPE_FIELDS (rt) ; field ; field = TREE_CHAIN (field))
      {
      /* Don't process any compiler-generated fields.  */
      if (DECL_ARTIFICIAL(field) && !DECL_IMPLICIT_TYPEDEF_P(field)) continue;

      /* A class or struct internally typedefs itself.  Don't
         output this extra typedef.  */
      if (!((TREE_CODE (field) == TYPE_DECL)
            && (TREE_TYPE (field) == rt)))
        {
        int id = xml_add_node (xdi, field, 1);
        if (id)
          {
          fprintf (xdi->file, "_%d ", id);
          }
        }
      }

    /* Output all the method declarations in the class.  */
    for (func = TYPE_METHODS (rt) ; func ; func = TREE_CHAIN (func))
      {
      int id;

      /* Don't process any compiler-generated functions except
         constructors, destructors, and the assignment operator.  */
      if (DECL_ARTIFICIAL(func)
          && !DECL_CONSTRUCTOR_P (func)
          && !DECL_DESTRUCTOR_P (func)
          && !DECL_ASSIGNMENT_OPERATOR_P (func)) continue;

      /* Don't output the cloned functions.  */
      if (DECL_CLONED_FUNCTION_P (func)) continue;

      id = xml_add_node (xdi, func, 1);
      if(id)
        {
        fprintf (xdi->file, "_%d ", id);
        }
      }

    /* TODO: List member template instantiations as members.  */

    fprintf (xdi->file, "\"");
    }

  /* Output all the base classes (compatibility with gccxml 0.6).  */
  if (dn->complete && COMPLETE_TYPE_P (rt) && TYPE_BINFO (rt))
    {
    tree binfo = TYPE_BINFO (rt);
    int n_baselinks = BINFO_N_BASE_BINFOS (binfo);
    tree accesses = BINFO_BASE_ACCESSES (binfo);
    int i;

    has_bases = (n_baselinks > 0)? 1:0;
    fprintf (xdi->file, " bases=\"");
    for (i = 0; i < n_baselinks; i++)
      {
      tree base_binfo = BINFO_BASE_BINFO(binfo, i);
      if (base_binfo)
        {
        /* Output this base class.  Default is public access.  */
        tree n_access = (accesses ? BINFO_BASE_ACCESS(binfo, i)
                                  : access_public_node);
        const char* access = "";
        if (n_access == access_protected_node) { access = "protected:"; }
        else if (n_access == access_private_node) { access = "private:"; }

        fprintf (xdi->file, "%s_%d ", access,
                 xml_add_node (xdi, BINFO_TYPE (base_binfo), 1));
        }
      }
    fprintf (xdi->file, "\"");
    }

  /* If there were no base classes, end the element now.  */
  if(!has_bases)
    {
    fprintf (xdi->file, "/>\n");
    return;
    }

  /* There are base classes.  Open the element for nested elements.  */
  fprintf (xdi->file, ">\n");

  /* Output all the base classes.  */
  if (dn->complete && COMPLETE_TYPE_P (rt) && TYPE_BINFO (rt))
    {
    tree binfo = TYPE_BINFO (rt);
    int n_baselinks = BINFO_N_BASE_BINFOS (binfo);
    tree accesses = BINFO_BASE_ACCESSES (binfo);
    int i;

    for (i = 0; i < n_baselinks; i++)
      {
      tree base_binfo = BINFO_BASE_BINFO(binfo, i);
      if (base_binfo)
        {
        /* Output this base class.  Default is public access.  */
        tree n_access = (accesses ? BINFO_BASE_ACCESS(binfo, i)
                                  : access_public_node);
        int is_virtual = BINFO_VIRTUAL_P (base_binfo);
        const char* access = "public";
        if (n_access == access_protected_node) { access = "protected"; }
        else if (n_access == access_private_node) { access = "private"; }

        fprintf (xdi->file,
                 "    <Base type=\"_%d\" access=\"%s\" virtual=\"%d\""
                 " offset=\"%d\"/>\n",
                 xml_add_node (xdi, BINFO_TYPE (base_binfo), 1),
                 access, is_virtual,
                 tree_low_cst (BINFO_OFFSET (base_binfo), 0));
        }
      }
    }

  fprintf (xdi->file, "  </%s>\n", tag);
}

static void
xml_document_add_element_record_type_base (xml_document_info_p xdi,
                                           xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Base";
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_access(e);
  xml_document_add_attribute(e, "virtual",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_attribute(e, "offset",
                             xml_document_attribute_type_integer,
                             xml_document_attribute_use_optional, "0");
}

static void
xml_document_add_element_record_type_helper (xml_document_info_p xdi,
                                             xml_document_element_p parent,
                                             const char* tag)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = tag;
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_optional);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     1);
  xml_document_add_attribute_abstract(e);
  xml_document_add_attribute_incomplete(e);
  xml_document_add_attribute_mangled(e);
  xml_document_add_attribute_demangled(e);
  xml_document_add_attribute_location(e, xml_document_attribute_use_required);
  xml_document_add_attribute_artificial(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
  xml_document_add_attribute_befriending(e);
  xml_document_add_attribute(e, "members",
                             xml_document_attribute_type_idrefs,
                             xml_document_attribute_use_optional, 0);
  xml_document_add_attribute(e, "bases",
                             xml_document_attribute_type_string,
                             xml_document_attribute_use_optional, 0);
  xml_document_add_element_record_type_base(xdi, e);
}

static void
xml_document_add_element_record_type (xml_document_info_p xdi,
                                      xml_document_element_p parent)
{
  xml_document_add_element_record_type_helper(xdi, parent, "Class");
  xml_document_add_element_record_type_helper(xdi, parent, "Struct");
  xml_document_add_element_record_type_helper(xdi, parent, "Union");
}

/*--------------------------------------------------------------------------*/
/* Output for a fundamental type.  */
static void
xml_output_fundamental_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <FundamentalType");
  xml_print_id_attribute (xdi, dn);
  /* Some fundamental types do not have names!  */
  if (TYPE_NAME (t))
    {
    xml_print_name_attribute (xdi, DECL_NAME (TYPE_NAME (t)));
    }
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  xml_print_size_attribute (xdi, t);
  xml_print_align_attribute (xdi, t);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_fundamental_type (xml_document_info_p xdi,
                                           xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "FundamentalType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
}

/*--------------------------------------------------------------------------*/
/* Output for a FUNCTION_TYPE.  */
static void
xml_output_function_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  tree arg_type;

  fprintf (xdi->file, "  <FunctionType");
  xml_print_id_attribute (xdi, dn);
  xml_print_returns_attribute (xdi, TREE_TYPE (t), dn->complete);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, ">\n");

  /* Prepare to iterator through argument list.  */
  arg_type = TYPE_ARG_TYPES (t);

  /* Print out the argument list for this function.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (xdi, NULL, arg_type, dn->complete);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(arg_type != void_list_node)
    {
    /* Function has variable number of arguments.  */
    xml_output_ellipsis (xdi);
    }

  fprintf (xdi->file, "  </FunctionType>\n");
}

static void
xml_document_add_element_function_type (xml_document_info_p xdi,
                                        xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "FunctionType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_returns(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_element_argument(xdi, e);
  xml_document_add_element_ellipsis(xdi, e);
}

/*--------------------------------------------------------------------------*/
/* Output for a METHOD_TYPE.  */
static void
xml_output_method_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  tree arg_type;
  tree this_type;

  fprintf (xdi->file, "  <MethodType");
  xml_print_id_attribute (xdi, dn);
  xml_print_base_type_attribute (xdi, TYPE_METHOD_BASETYPE (t), dn->complete);
  xml_print_returns_attribute (xdi, TREE_TYPE (t), dn->complete);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);

  /* Prepare to iterator through argument list.  */
  arg_type = TYPE_ARG_TYPES (t);

  /* We need to find out if the implicit argument points to a CV
     qualified type. */
  this_type = TREE_TYPE(TREE_VALUE (arg_type));
  if (this_type)
    {
    if (TYPE_READONLY (this_type))
      {
      fprintf (xdi->file, " const=\"1\"");
      }
    if (TYPE_VOLATILE (this_type))
      {
      fprintf (xdi->file, " volatile=\"1\"");
      }
    }

  fprintf (xdi->file, ">\n");

  /* Skip "this" argument.  */
  arg_type = TREE_CHAIN (arg_type);

  /* Print out the argument list for this method.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (xdi, NULL, arg_type, dn->complete);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(arg_type != void_list_node)
    {
    /* Method has variable number of arguments.  */
    xml_output_ellipsis (xdi);
    }

  fprintf (xdi->file, "  </MethodType>\n");
}

static void
xml_document_add_element_method_type (xml_document_info_p xdi,
                                      xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "MethodType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_base_type(e);
  xml_document_add_attribute_returns(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute(e, "const",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_attribute(e, "volatile",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_element_argument(xdi, e);
  xml_document_add_element_ellipsis(xdi, e);
}

/*--------------------------------------------------------------------------*/
/* Output for a POINTER_TYPE.  */
static void
xml_output_pointer_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <PointerType");
  xml_print_id_attribute (xdi, dn);
  xml_print_type_attribute (xdi, TREE_TYPE (t), 0);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  xml_print_size_attribute (xdi, t);
  xml_print_align_attribute (xdi, t);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_pointer_type (xml_document_info_p xdi,
                                       xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "PointerType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
}

/*--------------------------------------------------------------------------*/
/* Output for a REFERENCE_TYPE.  */
static void
xml_output_reference_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <ReferenceType");
  xml_print_id_attribute (xdi, dn);
  xml_print_type_attribute (xdi, TREE_TYPE (t), 0);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  xml_print_size_attribute (xdi, t);
  xml_print_align_attribute (xdi, t);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_reference_type (xml_document_info_p xdi,
                                         xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "ReferenceType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
}

/*--------------------------------------------------------------------------*/
/* Output for an OFFSET_TYPE.  */
static void
xml_output_offset_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <OffsetType");
  xml_print_id_attribute (xdi, dn);
  xml_print_base_type_attribute (xdi, TYPE_OFFSET_BASETYPE (t), dn->complete);
  xml_print_type_attribute (xdi, TREE_TYPE (t), dn->complete);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  xml_print_size_attribute (xdi, t);
  xml_print_align_attribute (xdi, t);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_offset_type (xml_document_info_p xdi,
                                      xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "OffsetType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_base_type(e);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
}

/*--------------------------------------------------------------------------*/
/* Output for an ARRAY_TYPE.  */
static void
xml_output_array_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <ArrayType");
  xml_print_id_attribute (xdi, dn);
  xml_print_array_attributes (xdi, t);
  xml_print_type_attribute (xdi, TREE_TYPE (t), dn->complete);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  xml_print_size_attribute (xdi, t);
  xml_print_align_attribute (xdi, t);
  fprintf (xdi->file, "/>\n");
}

static void
xml_document_add_element_array_type (xml_document_info_p xdi,
                                     xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "ArrayType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_array(e);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
}

/*--------------------------------------------------------------------------*/
/* Output for an ENUMERAL_TYPE.  */
static void
xml_output_enumeral_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  tree tv;

  fprintf (xdi->file, "  <Enumeration");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (TYPE_NAME (t)));
  xml_print_context_attribute (xdi, TYPE_NAME (t));
  xml_print_location_attribute (xdi, TYPE_NAME (t));
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  xml_print_artificial_attribute (xdi, TYPE_NAME (t));
  xml_print_size_attribute (xdi, t);
  xml_print_align_attribute (xdi, t);
  fprintf (xdi->file, ">\n");

  /* Output the list of possible values for the enumeration type.  */
  for (tv = TYPE_VALUES (t); tv ; tv = TREE_CHAIN (tv))
    {
    if(TREE_CODE (TREE_VALUE (tv)) == INTEGER_CST)
      {
      int value = TREE_INT_CST_LOW (TREE_VALUE (tv));
      fprintf (xdi->file,
               "    <EnumValue name=\"%s\" init=\"%d\"/>\n",
               xml_get_encoded_string ( TREE_PURPOSE(tv)), value);
      }
    else
      {
      fprintf (xdi->file, "  ");
      xml_output_unimplemented (xdi, TREE_VALUE (tv), 0,
                                "xml_output_enumeral_type");
      }
    }

  fprintf (xdi->file, "  </Enumeration>\n");
}

static void
xml_document_add_element_enum_value (xml_document_info_p xdi,
                                     xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "EnumValue";
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_init(e);
}

static void
xml_document_add_element_enumeral_type (xml_document_info_p xdi,
                                        xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "Enumeration";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_name(e, xml_document_attribute_use_required);
  xml_document_add_attribute_context(e, xml_document_attribute_use_required,
                                     1);
  xml_document_add_attribute_location(e, xml_document_attribute_use_required);
  xml_document_add_attribute_attributes(e);
  xml_document_add_attribute_artificial(e);
  xml_document_add_attribute_size(e);
  xml_document_add_attribute_align(e);
  xml_document_add_element_enum_value(xdi, e);
  xml_document_add_element_unimplemented(xdi, e);
}

/*--------------------------------------------------------------------------*/
/* Output for a cv-qualified type.  */
static void
xml_output_cv_qualified_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  if (t == TYPE_MAIN_VARIANT(t))
    {
    switch (TREE_CODE(t))
      {
      case UNION_TYPE:
      case QUAL_UNION_TYPE:
      case RECORD_TYPE:
        xml_output_record_type (xdi, t, dn);
        break;
      case ARRAY_TYPE:
        xml_output_array_type (xdi, t, dn);
        break;
      case POINTER_TYPE:
        xml_output_pointer_type (xdi, t, dn);
        break;
      case REFERENCE_TYPE:
        xml_output_reference_type (xdi, t, dn);
        break;
      case FUNCTION_TYPE:
        xml_output_function_type (xdi, t, dn);
        break;
      case METHOD_TYPE:
        xml_output_method_type (xdi, t, dn);
        break;
      case OFFSET_TYPE:
        xml_output_offset_type (xdi, t, dn);
        break;
      case ENUMERAL_TYPE:
        xml_output_enumeral_type (xdi, t, dn);
        break;
      case LANG_TYPE:
      case INTEGER_TYPE:
      case BOOLEAN_TYPE:
      case REAL_TYPE:
      case VOID_TYPE:
      case COMPLEX_TYPE:
        xml_output_fundamental_type (xdi, t, dn);
        break;
      default:
        break;
      }
    }
  else
    {
    /* Ignore the real index of this node and use the index of the
       unqualified version of the node with the extra characters.  */
    int qc = 0;
    int qv = 0;
    int qr = 0;
    int id = xml_get_idref(xdi, t, dn->complete, &qc, &qv, &qr);

    const char* c = qc? "c" : "";
    const char* v = qv? "v" : "";
    const char* r = qr? "r" : "";

    /* If for some reason there are no cv-qualifiers then just do not
       output the type.  Any reference to it will skip to the
       unqualified type anyway.  */
    if(!(qc||qv||qr))
      {
      return;
      }

    /* Create a special CvQualifiedType element to hold top-level
       cv-qualifiers for a real type node. */
    fprintf (xdi->file, "  <CvQualifiedType");
    fprintf (xdi->file, " id=\"_%d%s%s%s\"", id, c, v, r);

    /* Refer to the unqualified type.  */
    fprintf (xdi->file, " type=\"_%d\"", id);

    /* Add the cv-qualification attributes. */
    if (qc)
      {
      fprintf (xdi->file, " const=\"1\"");
      }
    if (qv)
      {
      fprintf (xdi->file, " volatile=\"1\"");
      }
    if (qr)
      {
      fprintf (xdi->file, " restrict=\"1\"");
      }
    fprintf (xdi->file, "/>\n");
    }
}

static void
xml_document_add_element_cv_qualified_type (xml_document_info_p xdi,
                                            xml_document_element_p parent)
{
  xml_document_element_p e = xml_document_add_subelement(xdi, parent, 0, 1);
  e->name = "CvQualifiedType";
  xml_document_add_attribute_id(e);
  xml_document_add_attribute_type(e);
  xml_document_add_attribute(e, "const",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_attribute(e, "volatile",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
  xml_document_add_attribute(e, "restrict",
                             xml_document_attribute_type_boolean,
                             xml_document_attribute_use_optional, "0");
}

/*--------------------------------------------------------------------------*/

/* When a class, struct, or union type is defined, it is automatically
   given a member typedef to itself.  Given a RECORD_TYPE or
   UNION_TYPE, this returns that field's name.  The return value is
   0 when the field cannot be found (such as for ENUMERAL_TYPE).  */
static tree
xml_find_self_typedef_name (tree rt)
{
  tree field;
  if(TREE_CODE (rt) == RECORD_TYPE ||
     TREE_CODE (rt) == UNION_TYPE ||
     TREE_CODE (rt) == QUAL_UNION_TYPE)
    {
    for (field = TYPE_FIELDS (rt) ; field ; field = TREE_CHAIN (field))
      {
      /* The field must be artificial because someone could have written
         their own typedef of the class's name.  */
      if ((TREE_CODE (field) == TYPE_DECL)
          && (TREE_TYPE (field) == rt)
          && DECL_ARTIFICIAL (field))
        {
        return DECL_NAME (field);
        }
      }
    }

  return 0;
}

/* Add the given typedef if it really came from user code.  */
static int
xml_add_typedef (xml_dump_info_p xdi, tree td, int complete)
{
  tree t;

  /* If the typedef points to its own name in the same context, ignore
     it.  */
  if(!DECL_ORIGINAL_TYPE (td)
     && TREE_TYPE (td) && TYPE_NAME (TREE_TYPE (td))
     && (DECL_NAME (td) == DECL_NAME (TYPE_NAME (TREE_TYPE (td))))
     && (DECL_CONTEXT (td) == DECL_CONTEXT (TYPE_NAME (TREE_TYPE (td)))))
    {
    /* Add the node that the typedef references instead.  */
    xml_add_node(xdi, TREE_TYPE (td), complete);
    return 0;
    }

  /* Find the typedef's real target type.  */
  if (DECL_ORIGINAL_TYPE (td))
    {
    t = DECL_ORIGINAL_TYPE (td);
    }
  else
    {
    t = TREE_TYPE (td);
    }

  if (TYPE_MAIN_VARIANT (t))
    {
    t = TYPE_MAIN_VARIANT (t);
    }

  /* Only add the typedef if its target will be dumped.  */
  if (xml_add_node (xdi, t, complete))
    {
    return xml_add_node_real (xdi, td, complete);
    }
  else
    {
    return 0;
    }
}

/* Dispatch output of a TYPE_DECL.  This is either a typedef, or a new
   type (class/struct/union) definition.  */
static int
xml_add_type_decl (xml_dump_info_p xdi, tree td, int complete)
{
  /* Get the type from the TYPE_DECL.  We don't want to use complete_type
     because it may modify something.  We are doing a read-only dump.  */
  tree t = TREE_TYPE (td);

  switch (TREE_CODE (t))
    {
    case ARRAY_TYPE:
    case LANG_TYPE:
    case POINTER_TYPE:
    case REFERENCE_TYPE:
    case INTEGER_TYPE:
    case VOID_TYPE:
    case BOOLEAN_TYPE:
    case REAL_TYPE:
    case COMPLEX_TYPE:
    case FUNCTION_TYPE:
    case TYPENAME_TYPE:
    case TEMPLATE_TYPE_PARM:
      /* A typedef to a predefined type.  */
      return xml_add_typedef (xdi, td, complete);
      break;
    case ENUMERAL_TYPE:
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
    case RECORD_TYPE:
      if ((TREE_CODE (t) == RECORD_TYPE) && TYPE_PTRMEMFUNC_P (t))
        {
        /* A typedef to a pointer to member.  */
        return xml_add_typedef (xdi, td, complete);
        }
      /* This is a complicated test to tell apart a real class declaration
         or definition from a typedef to it.  This must be able to detect
         the difference for the following cases:

            Code:                            Meaning:
         1.  struct A {};                     "struct A"
         2.  struct A {}; typedef A B;        "struct A" "typedef A B"
         3.  typedef struct {} A;             "struct A"
         4.  typedef struct A {} B;           "struct A" "typedef A B"
         5.  typedef struct A {} A;           "struct A"
         6.  struct A {}; typedef struct A A; "struct A"

         DECL_IMPLICIT_TYPEDEF_P will recognize the TYPE_DECL that
         points to the real class definition for cases 1, 2, 3, and 4,
         and is sufficient to make the decision.

         For cases 5 and 6, the second test makes sure the context of
         the TYPE_DECL matches the context of the RECORD_TYPE, which
         can only happen for a TYPE_DECL in the same scope as the
         class.  It then looks at the RECORD_TYPE to find its
         artificial member that is a typedef to its own class.  The
         name of this field is compared to the name of the original
         TYPE_DECL.  If it matches and the type's TYPE_NAME node is
         this TYPE_DECL, then we have found the class's definition.
         Otherwise, it is assumed that it is simply a typedef.  */
      else if (DECL_IMPLICIT_TYPEDEF_P (td)
               || (((DECL_CONTEXT (td) == TYPE_CONTEXT (t))
                     && (xml_find_self_typedef_name (t) == DECL_NAME (td))
                     && (TYPE_NAME (t) == td))))
        {
        /* This is a new class or enumeration type.  */
        return xml_add_node (xdi, t, complete);
        }
      else
        {
        /* A typedef to an existing class or enumeration type.  */
        return xml_add_typedef (xdi, td, complete);
        }
      break;
    default:
      {
      /* Add the node even though it is unimplemented.  */
      return xml_add_node_real (xdi, td, complete);
      }
    }
  return 0;
}

/* Dump for an OVERLOAD.  Dump all the functions in the overload list.   */
static int
xml_add_overload (xml_dump_info_p xdi, tree o, int complete)
{
  tree cur_overload = o;
  while (cur_overload)
    {
    xml_add_node (xdi, OVL_CURRENT (cur_overload), complete);
    cur_overload = OVL_NEXT (cur_overload);
    }
  return 0;
}

/* Recursively search a type node for template parameters.  */
static int
xml_find_template_parm (tree t)
{
  if(!t)
    {
    return 0;
    }

  switch (TREE_CODE (t))
    {
    /* A vector of template arguments on an instantiation.  */
    case TREE_VEC:
      {
      int i;
      for(i=0; i < TREE_VEC_LENGTH (t); ++i)
        {
        if(xml_find_template_parm (TREE_VEC_ELT (t, i)))
          {
          return 1;
          }
        }
      } break;

    /* A type list has nested types.  */
    case TREE_LIST:
      {
      if(xml_find_template_parm (TREE_PURPOSE (t)))
        {
        return 1;
        }
      return xml_find_template_parm (TREE_VALUE (t));
      } break;

    /* Template parameter types.  */
    case TEMPLATE_TYPE_PARM: return 1;
    case TEMPLATE_TEMPLATE_PARM: return 1;
    case TEMPLATE_PARM_INDEX: return 1;
    case TYPENAME_TYPE: return 1;
    case BOUND_TEMPLATE_TEMPLATE_PARM: return 1;

    /* A constant or variable declaration is encountered when a
       template instantiates another template using an enum or static
       const value that is not known until the outer template is
       instantiated.  */
    case CONST_DECL: return 1;
    case VAR_DECL: return 1;
    case FUNCTION_DECL: return 1;
    case OVERLOAD: return 1;
    case FIELD_DECL: return 1;

    /* A template deferred scoped lookup.  */
    case SCOPE_REF: return 1;

    /* A cast of a dependent expression.  */
    case CAST_EXPR: return 1;

    /* Types with nested types.  */
    case METHOD_TYPE:
    case FUNCTION_TYPE:
      {
      tree arg_type = TYPE_ARG_TYPES (t);
      if(xml_find_template_parm (TREE_TYPE (t)))
        {
        return 1;
        }
      while (arg_type && (arg_type != void_list_node))
        {
        if(xml_find_template_parm (arg_type))
          {
          return 1;
          }
        arg_type = TREE_CHAIN (arg_type);
        }
      } break;
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
    case RECORD_TYPE:
      {
      if ((TREE_CODE (t) == RECORD_TYPE) && TYPE_PTRMEMFUNC_P (t))
        {
        return xml_find_template_parm(TYPE_PTRMEMFUNC_FN_TYPE (t));
        }
      if (CLASSTYPE_TEMPLATE_INFO (t))
        {
        return xml_find_template_parm (CLASSTYPE_TI_ARGS (t));
        }
      }
    case REFERENCE_TYPE: return xml_find_template_parm (TREE_TYPE (t));
    case INDIRECT_REF: return xml_find_template_parm (TREE_TYPE (t));
    case COMPONENT_REF: return xml_find_template_parm (TREE_TYPE (t));
    case POINTER_TYPE: return xml_find_template_parm (TREE_TYPE (t));
    case ARRAY_TYPE: return xml_find_template_parm (TREE_TYPE (t));
    case OFFSET_TYPE:
      {
      return (xml_find_template_parm(TYPE_OFFSET_BASETYPE (t)) ||
              xml_find_template_parm (TREE_TYPE (t)));
      }
    case PTRMEM_CST:
      {
      return (xml_find_template_parm(PTRMEM_CST_CLASS (t)) ||
              xml_find_template_parm(PTRMEM_CST_MEMBER(t)));
      }

    /* Fundamental types have no nested types.  */
    case BOOLEAN_TYPE: return 0;
    case COMPLEX_TYPE: return 0;
    case ENUMERAL_TYPE: return 0;
    case INTEGER_TYPE: return 0;
    case LANG_TYPE: return 0;
    case REAL_TYPE: return 0;
    case VOID_TYPE: return 0;

    /* Template declarations are part of instantiations of template
       template parameters.  */
    case TEMPLATE_DECL: return 0;

    /* Unary expressions.  */
    case ALIGNOF_EXPR:
    case SIZEOF_EXPR:
    case ADDR_EXPR:
    case CONVERT_EXPR:
    case NOP_EXPR:
    case NEGATE_EXPR:
    case BIT_NOT_EXPR:
    case TRUTH_NOT_EXPR:
    case PREDECREMENT_EXPR:
    case PREINCREMENT_EXPR:
    case POSTDECREMENT_EXPR:
    case POSTINCREMENT_EXPR:
      return xml_find_template_parm (TREE_OPERAND (t, 0));

    /* Binary expressions.  */
    case COMPOUND_EXPR:
    case INIT_EXPR:
    case MODIFY_EXPR:
    case PLUS_EXPR:
    case MINUS_EXPR:
    case MULT_EXPR:
    case TRUNC_DIV_EXPR:
    case TRUNC_MOD_EXPR:
    case MIN_EXPR:
    case MAX_EXPR:
    case LSHIFT_EXPR:
    case RSHIFT_EXPR:
    case BIT_IOR_EXPR:
    case BIT_XOR_EXPR:
    case BIT_AND_EXPR:
    case TRUTH_ANDIF_EXPR:
    case TRUTH_ORIF_EXPR:
    case LT_EXPR:
    case LE_EXPR:
    case GT_EXPR:
    case GE_EXPR:
    case EQ_EXPR:
    case NE_EXPR:
    case EXACT_DIV_EXPR:
      return (xml_find_template_parm (TREE_OPERAND (t, 0))
              || xml_find_template_parm (TREE_OPERAND (t, 1)));

    /* Ternary expressions.  */
    case COND_EXPR:
      return (xml_find_template_parm (TREE_OPERAND (t, 0))
              || xml_find_template_parm (TREE_OPERAND (t, 1))
              || xml_find_template_parm (TREE_OPERAND (t, 2)));

    /* Other expressions.  */
    case TYPEOF_TYPE:
      return xml_find_template_parm (TYPEOF_TYPE_EXPR (t));
    case CALL_EXPR:
      {
      tree arg_expr;
      tree func_expr = TREE_OPERAND (t, 0);
      gcc_assert(func_expr);
      if (xml_find_template_parm(func_expr))
        {
        return 1;
        }
      arg_expr = TREE_OPERAND (t, 1);
      while (arg_expr)
        {
        if (xml_find_template_parm(arg_expr))
          {
          return 1;
          }
        arg_expr = TREE_CHAIN (arg_expr);
        }
      return 0;
      }
    case TEMPLATE_ID_EXPR:
      {
      tree argument_vec;
      tree template_expr = TREE_OPERAND (t, 0);
      gcc_assert(template_expr);
      if (xml_find_template_parm(template_expr))
        {
        return 1;
        }
      argument_vec = TREE_OPERAND (t, 1);
      if (argument_vec)
        {
        int i;
        for (i = 0; i < TREE_VEC_LENGTH(argument_vec); ++i)
          {
          tree argument_expr = TREE_VEC_ELT(argument_vec, i);
          if (xml_find_template_parm(argument_expr))
            {
            return 1;
            }
          }
        }
      return 0;
      }

    /* Other types that have no nested types.  */
    case INTEGER_CST: return 0;
    case STATIC_CAST_EXPR: return 0;
    default:
      fprintf(stderr, "xml_find_template_parm encountered unsupported type %s\n",
              tree_code_name[TREE_CODE (t)]);
    }
  return 0;
}

/* Dump for a TEMPLATE_DECL.  The set of specializations (including
   instantiations) is dumped.  */
static int
xml_add_template_decl (xml_dump_info_p xdi, tree td, int complete)
{
  tree tl;

  /* Dump the template specializations.  */
  for (tl = DECL_TEMPLATE_SPECIALIZATIONS (td);
       tl ; tl = TREE_CHAIN (tl))
    {
    tree ts = TREE_VALUE (tl);
    switch (TREE_CODE (ts))
      {
      case FUNCTION_DECL:
        xml_add_node (xdi, ts, complete);
        break;
      case TEMPLATE_DECL:
        break;
      default:
        /* xml_output_unimplemented (xdi, ts, 0,
          "xml_dump_template_decl SPECIALIZATIONS");  */
        break;
      }
    }

  /* Dump the template instantiations.  */
  for (tl = DECL_TEMPLATE_INSTANTIATIONS (td);
       tl ; tl = TREE_CHAIN (tl))
    {
    tree ts = TYPE_NAME (TREE_VALUE (tl));
    switch (TREE_CODE (ts))
      {
      case TYPE_DECL:
        /* Add the instantiation only if it is real.  */
        if (!xml_find_template_parm (TYPE_TI_ARGS(TREE_TYPE(ts))))
          {
          xml_add_node (xdi, ts, complete);
          }
        break;
      default:
        /* xml_output_unimplemented (xdi, ts, 0,
           "xml_dump_template_decl INSTANTIATIONS");  */
        break;
      }
    }

  /* Dump any member template instantiations.  */
  if (complete)
    {
    if(TREE_CODE (TREE_TYPE (td)) == RECORD_TYPE ||
       TREE_CODE (TREE_TYPE (td)) == UNION_TYPE ||
       TREE_CODE (TREE_TYPE (td)) == QUAL_UNION_TYPE)
      {
      for (tl = TYPE_FIELDS (TREE_TYPE (td)); tl; tl = TREE_CHAIN (tl))
        {
        if (TREE_CODE (tl) == TEMPLATE_DECL)
          {
          xml_add_template_decl (xdi, tl, 1);
          }
        }
      }
    }

  return 0;
}

/* Dump the given tree node.  */
void
xml_dump_tree_node (xml_dump_info_p xdi, tree n, xml_dump_node_p dn)
{
  switch (TREE_CODE (n))
    {
    case NAMESPACE_DECL:
      xml_output_namespace_decl (xdi, n, dn);
      break;
    case TYPE_DECL:
      xml_output_typedef (xdi, n, dn);
      break;
    case FUNCTION_DECL:
      xml_output_function_decl (xdi, n, dn);
      break;
    case VAR_DECL:
      xml_output_var_decl (xdi, n, dn);
      break;
    case FIELD_DECL:
      xml_output_field_decl (xdi, n, dn);
      break;
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
    case RECORD_TYPE:
    case ARRAY_TYPE:
    case POINTER_TYPE:
    case REFERENCE_TYPE:
    case FUNCTION_TYPE:
    case METHOD_TYPE:
    case OFFSET_TYPE:
    case ENUMERAL_TYPE:
    case LANG_TYPE:
    case INTEGER_TYPE:
    case BOOLEAN_TYPE:
    case REAL_TYPE:
    case VOID_TYPE:
    case COMPLEX_TYPE:
      xml_output_cv_qualified_type (xdi, n, dn);
      break;

    /* Let the following fall through to unimplemented for now.  */
    case RESULT_DECL:
    case USING_DECL:
      /* case THUNK_DECL: */
      /* This is compiler-generated.  Just ignore it.  */
      /* break; */
    case SCOPE_REF:
    case TEMPLATE_PARM_INDEX:
    case INTEGER_CST:
    case TYPENAME_TYPE:
    case TEMPLATE_TYPE_PARM:
    default:
      xml_output_unimplemented (xdi, n, dn, 0);
    }
}

/* If we enabled xml dump debugging, undefine the macro.  */
#if defined(xml_add_node)
# undef xml_add_node
#endif

/* Hook to suppress diagnostic messages during synthesize test.  */
extern int diagnostic_xml_synthesize_test;

/* Add tree node N to those encountered.  Return its index.  */
int
xml_add_node (xml_dump_info_p xdi, tree n, int complete)
{
  /* Skip internally generated declarations other than namespaces.  */
  if(TREE_CODE (n) != NAMESPACE_DECL &&
     DECL_P (n) && DECL_SOURCE_LINE(n) == 0)
    {
    /* If it turns out that internally generated namespaces other than the
       global namespace and std namespace should be disabled then change the namespace
       test above to

         n != global_namespace && !DECL_NAMESPACE_STD_P(n)

       which will enable just those internal declarations.  */
    return 0;
    }

  /* Skip invalid compiler-generated functions.  These can occur for
     code such as

       struct A { A(); const int a; };

     when the GCC parser produces the declaration but reports an error
     if the definition is actually needed.  */
  if (TREE_CODE (n) == FUNCTION_DECL &&
      DECL_ARTIFICIAL (n) && !DECL_INITIAL (n) &&
      (!DECL_REALLY_EXTERN (n) || DECL_INLINE (n)))
    {
    /* We try to synthesize this function but suppress error messages.  */
    diagnostic_xml_synthesize_test = 1;

    /* Taken from cp_finish_file.  */
    push_to_top_level ();
    input_location = DECL_SOURCE_LOCATION (n);
    synthesize_method (n);
    pop_from_top_level ();

    /* Error messages have been converted to GCCXML_DECL_ERROR marks.  */
    diagnostic_xml_synthesize_test = 0;
    }

  /* Skip synthesized invalid compiler-generated functions.  */
  if (TREE_CODE (n) == FUNCTION_DECL && GCCXML_DECL_ERROR (n))
    {
    return 0;
    }

  /* Some nodes don't need to be dumped and just refer to other nodes.
     These nodes should can have index zero because they should never
     be referenced.  */
  switch (TREE_CODE (n))
    {
    case NAMESPACE_DECL:
      return xml_add_node_real (xdi, n, complete);
      break;
    case TYPE_DECL:
      return xml_add_type_decl(xdi, n, complete);
      break;
    case TREE_LIST:
      return xml_add_overload (xdi, TREE_VALUE (n), complete);
      break;
    case OVERLOAD:
      return xml_add_overload (xdi, n, complete);
      break;
    case TEMPLATE_DECL:
      return xml_add_template_decl (xdi, n, complete);
      break;
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
    case RECORD_TYPE:
      if ((TREE_CODE (n) == RECORD_TYPE) && TYPE_PTRMEMFUNC_P (n))
        {
        /* Pointer-to-member-functions are stored in a RECORD_TYPE.  */
        return xml_add_node (xdi, TYPE_PTRMEMFUNC_FN_TYPE (n), complete);
        }
      else if (!CLASSTYPE_IS_TEMPLATE (n))
        {
        /* This is a struct or class type.  */
        return xml_add_node_real (xdi, n, complete);
        }
      else
        {
        /* This is a class template.  We don't want to dump it.  */
        return 0;
        }
      break;
    case CONST_DECL:
      /* Enumeration value constant.  Dumped by the enumeration type.  */
      return 0;
      break;
    default:
      /* This node must really be added.  */
      return xml_add_node_real (xdi, n, complete);
    }

  return 0;
}

/* Dump the queue of file names.  */
void xml_dump_files (xml_dump_info_p xdi)
{
  xml_file_queue_p fq;
  xml_file_queue_p next_fq;
  for(fq = xdi->file_queue; fq ; fq = next_fq)
    {
    fprintf (xdi->file, "  <File id=\"f%d\" name=\"%s\"/>\n",
             (unsigned int) fq->tree_node->value,
             IDENTIFIER_POINTER ((tree) fq->tree_node->key));
    next_fq = fq->next;
    free (fq);
    }
}

/*--------------------------------------------------------------------------*/

/* Starting in the given scope, look for the qualified name.  */
static tree
xml_lookup_start_node (tree scope, const char* qualified_name)
{
  tree node = 0;
  char* name = 0;
  int pos = 0;
  tree id;

  /* Parse off the first qualifier.  */
  while (qualified_name[pos] && (qualified_name[pos] != ':')) { ++pos; }
  name = xmalloc(pos+1);
  strncpy (name, qualified_name, pos);
  name[pos] = 0;
  id = get_identifier (name);
  free (name);

  /* Lookup the first qualifier.  */
  node = lookup_qualified_name (scope, id, 0, 1);

  if (!node) { return 0; }

  /* Find the start of the next qualifier, if any.  */
  while (qualified_name[pos] && (qualified_name[pos] == ':')) { ++pos; }

  /* If that was not the last qualifier, lookup the rest recursively.  */
  if (qualified_name[pos])
    {
    return xml_lookup_start_node (node, qualified_name+pos);
    }
  else
    {
    return node;
    }
}

/* Add the given node as a starting node.  */
static void
xml_add_start_node (xml_dump_info_p xdi, tree n)
{
  /* If a list was returned (from lookup_member), add all
     candidates.  */
  if ((TREE_CODE (n) == TREE_LIST)
      && (TREE_TYPE (n) == error_mark_node))
    {
    tree l = n;
    while (l)
      {
      xml_add_node (xdi, TREE_VALUE (l), 1);
      l = TREE_CHAIN (l);
      }
    }
  else
    {
    xml_add_node (xdi, n, 1);
    }
}

/* Parse the comma-separated list of start nodes.  Lookup and add each
   one.  */
void
xml_add_start_nodes (xml_dump_info_p xdi, const char* in_start_list)
{
  size_t len = strlen(in_start_list);
  int pos=0;
  char* start_list = (char*)xmalloc(len+1);
  char* cur_start = start_list;
  tree node = 0;
  strcpy(start_list, in_start_list);

  /* Parse out the comma-separated list.  */
  while (start_list[pos])
    {
    /* Split the list at each comma.  */
    if (start_list[pos] == ',')
      {
      start_list[pos] = 0;

      /* Add the node for this list entry.  */
      node = xml_lookup_start_node (global_namespace, cur_start);
      if (node)
        {
        xml_add_start_node (xdi, node);
        }

      /* Start the next list entry.  */
      cur_start = start_list+pos+1;
      }
    ++pos;
    }

  /* Add the node for the last list entry.  */
  node = xml_lookup_start_node (global_namespace, cur_start);
  if (node)
    {
    xml_add_start_node (xdi, node);
    }

  free (start_list);
}

/*--------------------------------------------------------------------------*/

/* Convert the identifier IN_ID to an XML encoded form by passing its string
   to xml_get_encoded_string_from_string().  */
const char*
xml_get_encoded_string (tree in_id)
{
  if(in_id)
    return xml_get_encoded_string_from_string (IDENTIFIER_POINTER (in_id));
  else
    return "";
}

#define XML_AMPERSAND     "&amp;"
#define XML_LESS_THAN     "&lt;"
#define XML_GREATER_THAN  "&gt;"
#define XML_SINGLE_QUOTE  "&apos;"
#define XML_DOUBLE_QUOTE  "&quot;"

const char*
xml_get_encoded_string_from_string (const char* in_str)
{
  return IDENTIFIER_POINTER (xml_get_encoded_identifier_from_string (in_str));
}

/* Convert the name IN_STR to an XML encoded form.
   This replaces '&', '<', and '>'
   with their corresponding unicode character references.  */
const char*
xml_escape_string(const char* in_str)
{
  int length_increase = 0;
  const char* inCh;
  char* outCh;
  char* newStr;

  /* Count special characters and calculate extra length needed for encoded
     form of string.  */
  for(inCh = in_str; *inCh != '\0' ; ++inCh)
    {
    switch (*inCh)
      {
      case '&': length_increase += strlen(XML_AMPERSAND)-1; break;
      case '<': length_increase += strlen(XML_LESS_THAN)-1; break;
      case '>': length_increase += strlen(XML_GREATER_THAN)-1; break;
      case '\'': length_increase += strlen(XML_SINGLE_QUOTE)-1; break;
      case '"': length_increase += strlen(XML_DOUBLE_QUOTE)-1; break;
      }
    }

  newStr = (char*) xmalloc (strlen (in_str) + length_increase + 1);
  outCh = newStr;

  /* Create encoded form of string.  */
  for(inCh = in_str ; *inCh != '\0' ; ++inCh)
    {
    switch (*inCh)
      {
      case '&': strcpy (outCh, XML_AMPERSAND); outCh += strlen(XML_AMPERSAND); break;
      case '<': strcpy (outCh, XML_LESS_THAN);  outCh += strlen(XML_LESS_THAN); break;
      case '>': strcpy (outCh, XML_GREATER_THAN);  outCh += strlen(XML_GREATER_THAN); break;
      case '\'': strcpy (outCh, XML_SINGLE_QUOTE);  outCh += strlen(XML_SINGLE_QUOTE); break;
      case '"': strcpy (outCh, XML_DOUBLE_QUOTE);  outCh += strlen(XML_DOUBLE_QUOTE); break;
      default: *outCh++ = *inCh;
      }
    }

  *outCh = '\0';

  return newStr;
}

/* Convert the name IN_STR to an XML encoded form.
   This replaces '&', '<', and '>'
   with their corresponding unicode character references.  */
tree
xml_get_encoded_identifier_from_string (const char* in_str)
{
  char* newStr = xml_escape_string( in_str );
  tree id;

  /* Ask for "identifier" of this string and free our own copy of the
     memory.  */
  id = get_identifier(newStr);
  free(newStr);
  return id;
}

#undef XML_AMPERSAND
#undef XML_LESS_THAN
#undef XML_GREATER_THAN
#undef XML_SINGLE_QUOTE
#undef XML_DOUBLE_QUOTE

/*--------------------------------------------------------------------------*/
/* Called for all identifiers in the symbol table.  */
static int xml_fill_all_decls(struct cpp_reader* reader, hashnode node,
                              const void* user_data)
{
  /* Get the bindings for the current identifier.  */
  tree id = HT_IDENT_TO_GCC_IDENT(node);
  cxx_binding* binding = IDENTIFIER_NAMESPACE_BINDINGS(id);

  /* Avoid unused parameter warnings.  */
  (void)reader;
  (void)user_data;

  /* For each binding of this symbol add the declaration to the vector
     of all declarations in the corresponding scope.  */
  for(;binding; binding = binding->previous)
    {
    if(binding->value)
      {
      VEC_safe_push (tree, gc, binding->scope->all_decls, binding->value);
      }
    if(binding->type)
      {
      VEC_safe_push (tree, gc, binding->scope->all_decls, binding->type);
      }
    }
  return 1;
}

/*--------------------------------------------------------------------------*/
/* Check at

http://tools.decisionsoft.com/schemaValidate.html
http://www.xmlfiles.com/dtd/dtd_attributes.asp
http://www.w3schools.com/schema/schema_complex_subst.asp

*/
/*
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
           targetNamespace="http://www.w3schools.com"
           xmlns="http://www.w3schools.com"
           elementFormDefault="qualified">
 <xs:element name="to" type="xs:string"/>
 <xs:element name="note">
  <xs:complexType>
   <xs:sequence><!-- How to specify zero or more in any order? -->
    <xs:element ref="to"/>
    <xs:element name="from" type="xs:string" minOccurs="0" maxOccurs="unbounded"/>
   </xs:sequence>
   <xs:attribute name="lang1" type="xs:ID" use="required"/>
   <xs:attribute name="lang2" type="xs:IDREF" use="optional"/>
  </xs:complexType>
 </xs:element>
</xs:schema>

   <xs:element name="note" type="xs:string" minOccurs="0"/>
   <xs:group ref="xs:facets" minOccurs="0" maxOccurs="unbounded"/>
*/

static void
xml_document_generate_indent(xml_document_info_p xdi, int indent)
{
  int i;
  for (i=0; i < indent; ++i)
    {
    fprintf (xdi->file, " ");
    }
}

static void
xml_document_generate_attribute(xml_document_info_p xdi,
                                xml_document_element_p element,
                                xml_document_attribute_p attribute,
                                int indent)
{
  if(xdi->format == xml_document_format_dtd)
    {
    fprintf (xdi->file, "<!ATTLIST %s %s %s #%s>\n",
             element->name, attribute->name,
             xml_document_dtd_types[attribute->type],
             xml_document_dtd_uses[attribute->use]);
    }
  else if(xdi->format == xml_document_format_schema)
    {
    xml_document_generate_indent(xdi, indent);
    fprintf (xdi->file, "<xs:attribute name=\"%s\" type=\"%s\" use=\"%s\"/>",
             attribute->name,
             xml_document_schema_types[attribute->type],
             xml_document_schema_uses[attribute->use]);
    }
}

static void
xml_document_generate_element(xml_document_info_p xdi,
                              xml_document_element_p element, int indent)
{
  int i;
  if(xdi->format == xml_document_format_dtd)
    {
    fprintf (xdi->file, "<!ELEMENT %s (", element->name);
    if(element->num_subelements > 0)
      {
      char next[] = ",\n                                                    ";
      const char* pnext = "";
      next[strlen(element->name) + 12 + 2] = '\0';
      for(i=0; i < element->num_subelements; ++i)
        {
        fprintf (xdi->file, "%s%s", pnext,
                 element->subelements[i].element->name);
        if(element->subelements[i].required &&
           element->subelements[i].repeatable)
          {
          fprintf (xdi->file, "+");
          }
        else if(element->subelements[i].repeatable)
          {
          fprintf (xdi->file, "*");
          }
        else if(!element->subelements[i].required)
          {
          fprintf (xdi->file, "?");
          }
        pnext = next;
        }
      }
    else
      {
      fprintf (xdi->file, "EMPTY", element);
      }
    fprintf (xdi->file, ")>\n");
    }
  else if(xdi->format == xml_document_format_schema)
    {
    xml_document_generate_indent(xdi, indent);
    fprintf (xdi->file, "<xs:element name=\"%s\">\n", element);
    xml_document_generate_indent(xdi, indent);
    fprintf (xdi->file, " <xs:complexType>\n");
    }

  for(i=0; i < element->num_attributes; ++i)
    {
    xml_document_generate_attribute(xdi, element, &element->attributes[i],
                                    indent+3);
    }

  for(i=0; i < element->num_subelements; ++i)
    {
    xml_document_generate_element(xdi, element->subelements[i].element,
                                  indent+3);
    }

  if(xdi->format == xml_document_format_schema)
    {
    xml_document_generate_indent(xdi, indent);
    fprintf (xdi->file, " </xs:complexType>\n");
    xml_document_generate_indent(xdi, indent);
    fprintf (xdi->file, "</xs:element>\n");
    }
}

/* Main XML documentation generation function.  */
void
do_xml_document (const char* dtd_name, const char* schema_name)
{
  /* Record the documentation specification.  */
  xml_document_info xdi;
  xml_document_element_p element;
  xml_document_subelement_p subelement;
  memset(&xdi, 0, sizeof(xdi));

  element = &xdi.elements[xdi.num_elements++];
  element->name = "GCC-XML";
  xml_document_add_element_fundamental_type (&xdi, element);
  xml_document_add_element_unimplemented (&xdi, element);
  xml_document_add_element_namespace_decl (&xdi, element);
  xml_document_add_element_typedef (&xdi, element);
  xml_document_add_element_function (&xdi, element);
  xml_document_add_element_var_decl (&xdi, element);
  xml_document_add_element_field_decl (&xdi, element);
  xml_document_add_element_record_type (&xdi, element);
  xml_document_add_element_fundamental_type (&xdi, element);
  xml_document_add_element_function_type (&xdi, element);
  xml_document_add_element_method_type (&xdi, element);
  xml_document_add_element_pointer_type (&xdi, element);
  xml_document_add_element_reference_type (&xdi, element);
  xml_document_add_element_offset_type (&xdi, element);
  xml_document_add_element_array_type (&xdi, element);
  xml_document_add_element_enumeral_type (&xdi, element);
  xml_document_add_element_cv_qualified_type (&xdi, element);

  if(dtd_name)
    {
    /* Generate the DTD file.  */
    xdi.file = fopen (dtd_name, "w");
    xdi.format = xml_document_format_dtd;
    if (xdi.file)
      {
      xml_document_generate_element(&xdi, &xdi.elements[0], 0);
      }
    else
      {
      error ("could not open xml-dtd file `%s'", dtd_name);
      }
    }
}
