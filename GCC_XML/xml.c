/*=========================================================================

  Program:   GCC-XML
  Module:    xml.c
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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

/* Decide how to call cp_error.  */
#if defined(GCC_XML_GCC_VERSION) && (GCC_XML_GCC_VERSION >= 0x030100)
# define XML_CP_ERROR cp_error_at
#else
# define XML_CP_ERROR cp_error
# define XML_HAVE_FAKE_STD_NODE
#endif

/* Decide how to get the attributes node from a declaration.  */
#if defined(GCC_XML_GCC_VERSION) && (GCC_XML_GCC_VERSION >= 0x030100)
# define GCC_XML_DECL_ATTRIBUTES(n) DECL_ATTRIBUTES(n)
#else
# define GCC_XML_DECL_ATTRIBUTES(n) DECL_MACHINE_ATTRIBUTES(n)
#endif

/* Decide how to access base classes.  */
#if !defined(GCC_XML_GCC_VERSION) || (GCC_XML_GCC_VERSION < 0x030400)
# define XML_PRE_3_4_TREE_VIA_PUBLIC
#endif

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

/* Return non-zero if the given _DECL is an internally generated decl.  */
#define DECL_INTERNAL_P(d) (DECL_SOURCE_LINE(d) == 0)

void do_xml_output PARAMS ((const char*));

static int xml_add_node PARAMS((xml_dump_info_p, tree, int));
static void xml_dump PARAMS((xml_dump_info_p));
static int xml_queue_incomplete_dump_nodes PARAMS((splay_tree_node, void*));
static void xml_dump_tree_node PARAMS((xml_dump_info_p, tree, xml_dump_node_p));
static void xml_dump_files PARAMS((xml_dump_info_p));

static void xml_add_start_nodes PARAMS((xml_dump_info_p, const char*));

static const char* xml_get_encoded_string PARAMS ((tree));
static const char* xml_get_encoded_string_from_string PARAMS ((const char*));
static tree xml_get_encoded_identifier_from_string PARAMS ((const char*));

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

  /* Open the XML output file.  */
  file = fopen (filename, "w");
  if (!file)
    {
    XML_CP_ERROR ("could not open xml-dump file `%s'", filename);
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
#if defined(GCCXML_VERSION_FULL)
  fprintf (file, "<GCC_XML version=\"" GCCXML_VERSION_FULL "\">\n");
#else
  fprintf (file, "<GCC_XML>\n");
#endif

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
    XML_CP_ERROR ("XML dump bug: complete node added during incomplete phase.\n");
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

/* ------------------------------------------------------------------------ */

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

/* Print the XML attribute endline="line" for the given COMPOUND_STMT.  */
static void
xml_print_endline_attribute (xml_dump_info_p xdi, tree stmt)
{
  fprintf (xdi->file, " endline=\"%d\"", STMT_LINENO(stmt));
}

/* Print the XML attribute id="..." for the given node.  */
static void
xml_print_id_attribute (xml_dump_info_p xdi, xml_dump_node_p dn)
{
  fprintf (xdi->file, " id=\"_%d\"", dn->index);
}

/* Print the XML attribute name="..." for the given node.  */
static void
xml_print_name_attribute (xml_dump_info_p xdi, tree n)
{
  const char* name = xml_get_encoded_string (n);
  fprintf (xdi->file, " name=\"%s\"", name);
}

/* Print the XML attribute mangled="..." for the given node.  */
static void
xml_print_mangled_attribute (xml_dump_info_p xdi, tree n)
{
  if (DECL_NAME (n) &&
      DECL_ASSEMBLER_NAME (n) &&
      DECL_ASSEMBLER_NAME (n) != DECL_NAME (n))
    {
    const char* name = xml_get_encoded_string (DECL_ASSEMBLER_NAME (n));
    fprintf (xdi->file, " mangled=\"%s\"", name);
    }
}

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

/* Print the XML attribute returns="tid" for the given function type.  */
static void
xml_print_returns_attribute (xml_dump_info_p xdi, tree t, int complete)
{
  fprintf (xdi->file, " returns=\"");
  xml_print_type_idref (xdi, t, complete);
  fprintf (xdi->file, "\"");
}

/* Print XML attribute basetype="..." with the given type.  */
static void
xml_print_base_type_attribute (xml_dump_info_p xdi, tree t, int complete)
{
  fprintf (xdi->file, " basetype=\"_%d\"", xml_add_node (xdi, t, complete));
}

/* Print the XML attribute context="..." for the given node.  */
static void
xml_print_context_attribute (xml_dump_info_p xdi, tree n)
{
  if(n != global_namespace)
    {
    fprintf (xdi->file, " context=\"_%d\"",
             xml_add_node (xdi, CP_DECL_CONTEXT (n), 0));
    }
}

/* Print the XML attribute access="..." for the given decl.  */
static void
xml_print_access_attribute (xml_dump_info_p xdi, tree d)
{
  tree context = CP_DECL_CONTEXT (d);
  if (context && TYPE_P(context))
    {
    if (TREE_PRIVATE (d))
      {
      fprintf (xdi->file, " access=\"private\"");
      }
    else if (TREE_PROTECTED (d))
      {
      fprintf (xdi->file, " access=\"protected\"");
      }
    else
      {
      /* Default for access attribute is public.  */
      /* fprintf (xdi->file, " access=\"public\"");  */
      }
    }
}

/* Print XML attribute const="1" for const methods.  */
static void
xml_print_const_method_attribute (xml_dump_info_p xdi, tree fd)
{
  if (DECL_CONST_MEMFUNC_P (fd))
    {
    fprintf (xdi->file, " const=\"1\"");
    }
}

/* Print XML attribute static="1" for static methods.  */
static void
xml_print_static_method_attribute (xml_dump_info_p xdi, tree fd)
{
  if (!DECL_NONSTATIC_MEMBER_FUNCTION_P (fd))
    {
    fprintf (xdi->file, " static=\"1\"");
    }
}

/* Print XML attributes virtual="" and pure_virtual="" for a decl.  */
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
}

/* Print the XML attribute extern="1" if the given decl is external.  */
static void
xml_print_extern_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_EXTERNAL (d))
    {
    fprintf (xdi->file, " extern=\"1\"");
    }
}

/* Print the XML attribute extern="1" if the given decl is external.  */
static void
xml_print_function_extern_attribute (xml_dump_info_p xdi, tree fd)
{
  if (DECL_REALLY_EXTERN (fd))
    {
    fprintf (xdi->file, " extern=\"1\"");
    }
}

/* Print XML attribute for a default argument.  */
static void
xml_print_default_argument_attribute (xml_dump_info_p xdi, tree t)
{
  const char* value;

  switch (TREE_CODE(t))
    {
    case CAST_EXPR:
      value = xml_get_encoded_string_from_string ("<gccxml-cast-expr>");
      break;
    default:
      value = xml_get_encoded_string_from_string (expr_as_string (t, 0));
      break;
    };
  fprintf (xdi->file, " default=\"%s\"", value);
}

/* Print XML attribute init="..." for a variable initializer.  */
static void
xml_print_init_attribute (xml_dump_info_p xdi, tree t)
{
  const char* value;

  if (!t || (t == error_mark_node)) return;

  value = xml_get_encoded_string_from_string (expr_as_string (t, 0));
  fprintf (xdi->file, " init=\"%s\"", value);
}

/* Print the XML attribute incomplete="..." for the given type.  */
static void
xml_print_incomplete_attribute (xml_dump_info_p xdi, tree t)
{
  if (!COMPLETE_TYPE_P (t))
    {
    fprintf (xdi->file, " incomplete=\"1\"");
    }
}

/* Print the XML attribute abstract="..." for the given type.  */
static void
xml_print_abstract_attribute (xml_dump_info_p xdi, tree t)
{
  if (CLASSTYPE_PURE_VIRTUALS (t) != 0)
    {
    fprintf (xdi->file, " abstract=\"1\"");
    }
}

/* Print XML empty tag for an ellipsis at the end of an argument list.  */
static void
xml_output_ellipsis (xml_dump_info_p xdi)
{
  fprintf (xdi->file,
           "    <Ellipsis/>\n");
}

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
    fprintf(xdi->file, " attributes=\"");
    for(attribute = attributes1; attribute;
        attribute = TREE_CHAIN(attribute))
      {
      fprintf(xdi->file, "%s%s", space,
              xml_get_encoded_string(TREE_PURPOSE (attribute)));
      space = " ";
      }
    for(attribute = attributes2; attribute;
        attribute = TREE_CHAIN(attribute))
      {
      fprintf(xdi->file, "%s%s", space,
              xml_get_encoded_string(TREE_PURPOSE (attribute)));
      space = " ";
      }
    fprintf(xdi->file, "\"");
    }
}

/* Print XML attribute artificial="1" for compiler generated methods.  */
static void
xml_print_artificial_attribute(xml_dump_info_p xdi, tree d)
{
  if (DECL_ARTIFICIAL (d))
    {
    fprintf (xdi->file, " artificial=\"1\"");
    }
}

/* Print XML empty tag describing an unimplemented TREE_CODE that has been
   encountered.  */
static void
xml_output_unimplemented (xml_dump_info_p xdi, tree t, xml_dump_node_p dn,
                          const char* where)
{
  int tree_code = TREE_CODE (t);
  fprintf (xdi->file,
           "  <Unimplemented id=\"_%d\" tree_code=\"%d\""
           " tree_code_name=\"%s\" node=\"%p\"",
           (dn? dn->index : 0),
           tree_code, tree_code_name [tree_code], t);
  if (where)
    {
    fprintf (xdi->file, " function=\"%s\"", where);
    }
  fprintf (xdi->file, "/>\n");
}

/* ------------------------------------------------------------------------ */

/* Dump a NAMESPACE_DECL.  */
static void
xml_output_namespace_decl (xml_dump_info_p xdi, tree ns, xml_dump_node_p dn)
{
  /* Only walk a real namespace.  */
  if (!DECL_NAMESPACE_ALIAS (ns))
    {
    fprintf (xdi->file, "  <Namespace");
    xml_print_id_attribute (xdi, dn);
    xml_print_name_attribute (xdi, DECL_NAME (ns));
    xml_print_context_attribute (xdi, ns);
    xml_print_attributes_attribute (xdi, GCC_XML_DECL_ATTRIBUTES(ns), 0);

    /* If complete dump, walk the namespace.  */
    if(dn->complete)
      {
      tree cur_decl;
      fprintf (xdi->file, " members=\"");
      for (cur_decl = cp_namespace_decls(ns); cur_decl;
           cur_decl = TREE_CHAIN (cur_decl))
        {
        if (!DECL_INTERNAL_P (cur_decl))
          {
          int id = xml_add_node (xdi, cur_decl, 1);
          if (id)
            {
            fprintf (xdi->file, "_%d ", id);
            }
          }
        }
#if defined(GCC_XML_GCC_VERSION) && (GCC_XML_GCC_VERSION >= 0x030300)
      // Add child namespaces
      for (cur_decl = cp_namespace_namespaces(ns); cur_decl;
           cur_decl = TREE_CHAIN (cur_decl))
        {
        if (!DECL_INTERNAL_P (cur_decl))
          {
          int id = xml_add_node (xdi, cur_decl, 1);
          if (id)
            {
            fprintf (xdi->file, "_%d ", id);
            }
          }
        }
#endif

      fprintf (xdi->file, "\"");
      }

    xml_print_mangled_attribute (xdi, ns);
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
    fprintf (xdi->file, "/>\n");
    }
}

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
  fprintf (xdi->file, "/>\n");
}

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
    xml_print_type_attribute (xdi, TREE_TYPE (pd), complete);
    }
  else
    {
    xml_print_type_attribute (xdi, TREE_VALUE (tl), complete);
    }

  if (TREE_PURPOSE (tl))
    {
    xml_print_default_argument_attribute (xdi, TREE_PURPOSE (tl));
    }

  fprintf (xdi->file, "/>\n");
}

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
  tree body = DECL_SAVED_TREE(fd);
  int do_returns = 0;
  int do_access = 0;
  int do_const = 0;
  int do_virtual = 0;
  int do_static = 0;
  int do_artificial = 0;

  /* Print out the begin tag for this type of function.  */
  if (DECL_CONSTRUCTOR_P (fd))
    {
    /* A class constructor.  */
    tag = "Constructor"; do_access = 1; do_artificial = 1;
    }
  else if (DECL_DESTRUCTOR_P (fd))
    {
    /* A class destructor.  */
    tag = "Destructor"; do_access = 1; do_virtual = 1; do_artificial = 1;
    }
  else if (DECL_OVERLOADED_OPERATOR_P (fd))
    {
    if (DECL_CONV_FN_P (fd))
      {
      /* A type-conversion operator in a class.  */
      tag = "Converter";
      do_returns = 1; do_access = 1; do_const = 1; do_virtual = 1;
      }
    else
      {
      if (DECL_FUNCTION_MEMBER_P (fd))
        {
        /* An operator in a class.  */
        tag = "OperatorMethod";
        name = xml_reverse_opname_lookup (DECL_NAME (fd));
        do_returns = 1; do_access = 1; do_const = 1; do_virtual = 1;
        do_static = 1;
        }
      else
        {
        /* An operator in a namespace.  */
        tag = "OperatorFunction";
        name = xml_reverse_opname_lookup (DECL_NAME (fd));
        do_returns = 1;
        }
      }
    }
  else
    {
    if (DECL_FUNCTION_MEMBER_P (fd))
      {
      /* A member of a class.  */
      tag = "Method"; do_returns = 1; do_access = 1; do_const = 1;
      do_virtual = 1; do_static = 1;
      }
    else
      {
      /* A member of a namespace.  */
      tag = "Function"; do_returns = 1;
      }
    }

  fprintf (xdi->file, "  <%s", tag);
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, name);

  if(do_returns)
    {
    xml_print_returns_attribute (xdi, TREE_TYPE (TREE_TYPE (fd)), dn->complete);
    }
  if(do_access)  xml_print_access_attribute (xdi, fd);
  if(do_const)   xml_print_const_method_attribute (xdi, fd);
  if(do_virtual) xml_print_virtual_method_attributes (xdi, fd);
  if(do_static)  xml_print_static_method_attribute (xdi, fd);
  if(do_artificial)  xml_print_artificial_attribute (xdi, fd);
  xml_print_throw_attribute (xdi, TREE_TYPE (fd), dn->complete);
  xml_print_context_attribute (xdi, fd);
  xml_print_mangled_attribute (xdi, fd);
  xml_print_location_attribute (xdi, fd);
  if(body)
    {
    xml_print_endline_attribute (xdi, body);
    }
  xml_print_function_extern_attribute (xdi, fd);
  xml_print_attributes_attribute (xdi, GCC_XML_DECL_ATTRIBUTES(fd),
                                  TYPE_ATTRIBUTES(TREE_TYPE(fd)));

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
  xml_print_access_attribute (xdi, vd);
  xml_print_mangled_attribute (xdi, vd);
  xml_print_location_attribute (xdi, vd);
  xml_print_extern_attribute (xdi, vd);
  xml_print_artificial_attribute (xdi, vd);
  xml_print_attributes_attribute (xdi, GCC_XML_DECL_ATTRIBUTES(vd), 0);
  fprintf (xdi->file, "/>\n");
}

/* Output for a FIELD_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
static void
xml_output_field_decl (xml_dump_info_p xdi, tree fd, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <Field");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (fd));
  xml_print_access_attribute (xdi, fd);
  /* TODO: handle bit field case.  */
  xml_print_type_attribute (xdi, TREE_TYPE (fd), dn->complete);
  xml_print_context_attribute (xdi, fd);
  xml_print_mangled_attribute (xdi, fd);
  xml_print_location_attribute (xdi, fd);
  xml_print_attributes_attribute (xdi, GCC_XML_DECL_ATTRIBUTES(fd), 0);
  fprintf (xdi->file, "/>\n");
}

/* Output a RECORD_TYPE that is not a pointer-to-member-function.
   Prints beginning and ending tags, and all class member declarations
   between.  Also handles a UNION_TYPE.  */
static void
xml_output_record_type (xml_dump_info_p xdi, tree rt, xml_dump_node_p dn)
{
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
  xml_print_name_attribute (xdi, DECL_NAME (TYPE_NAME (rt)));
  xml_print_context_attribute (xdi, TYPE_NAME (rt));
  xml_print_access_attribute (xdi, TYPE_NAME (rt));
  xml_print_abstract_attribute (xdi, rt);
  xml_print_incomplete_attribute (xdi, rt);
  xml_print_mangled_attribute (xdi, TYPE_NAME (rt));
  xml_print_location_attribute (xdi, TYPE_NAME (rt));
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(rt), 0);

  if (dn->complete && COMPLETE_TYPE_P (rt))
    {
    fprintf (xdi->file, " members=\"");
    /* Output all the non-method declarations in the class.  */
    for (field = TYPE_FIELDS (rt) ; field ; field = TREE_CHAIN (field))
      {
      /* Don't process any internally generated declarations.  */
      if (DECL_INTERNAL_P (field)) continue;

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

      /* Don't process any internally generated declarations.  */
      if (DECL_INTERNAL_P (func)) continue;

      /* Don't process any compiler-generated functions except constructors
         and destructors.  */
      if (DECL_ARTIFICIAL(func)
          && !DECL_CONSTRUCTOR_P (func)
          && !DECL_DESTRUCTOR_P (func)) continue;

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

  /* Output all the base classes.  */
  if (dn->complete && COMPLETE_TYPE_P (rt))
    {
    tree binfo = TYPE_BINFO (rt);
    tree binfos = BINFO_BASETYPES (binfo);
    int n_baselinks = binfos? TREE_VEC_LENGTH (binfos) : 0;
#if !defined (XML_PRE_3_4_TREE_VIA_PUBLIC)
    tree accesses = BINFO_BASEACCESSES (binfo);
#endif
    int i;

    fprintf (xdi->file, " bases=\"");
    for (i = 0; i < n_baselinks; i++)
      {
      tree base_binfo = TREE_VEC_ELT (binfos, i);
      if (base_binfo)
        {
        /* Output this base class.  Default is public access.  */
#if defined (XML_PRE_3_4_TREE_VIA_PUBLIC)
        const char* access = 0;
        if (TREE_VIA_PUBLIC (base_binfo)) { access = ""; }
        else if (TREE_VIA_PROTECTED (base_binfo)) { access = "protected:"; }
        else { access="private:"; }
#else
        tree n_access = (accesses ? TREE_VEC_ELT (accesses, i)
                                  : access_public_node);
        const char* access = "";
        if (n_access == access_protected_node) { access = "protected:"; }
        else if (n_access == access_private_node) { access = "private:"; }
#endif

        fprintf (xdi->file, "%s_%d ", access,
                 xml_add_node (xdi, BINFO_TYPE (base_binfo), 1));
        }
      }
    fprintf (xdi->file, "\"");
    }

  fprintf (xdi->file, "/>\n");
}

/* Output for a fundamental type.  */
static void
xml_output_fundamental_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <FundamentalType");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (TYPE_NAME (t)));
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, "/>\n");
}

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

  /* We need to find out if the implicity argument points to a CV
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

/* Output for a POINTER_TYPE.  */
static void
xml_output_pointer_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <PointerType");
  xml_print_id_attribute (xdi, dn);
  xml_print_type_attribute (xdi, TREE_TYPE (t), 0);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, "/>\n");
}

/* Output for a REFERENCE_TYPE.  */
static void
xml_output_reference_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <ReferenceType");
  xml_print_id_attribute (xdi, dn);
  xml_print_type_attribute (xdi, TREE_TYPE (t), 0);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, "/>\n");
}

/* Output for an OFFSET_TYPE.  */
static void
xml_output_offset_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <OffsetType");
  xml_print_id_attribute (xdi, dn);
  xml_print_base_type_attribute (xdi, TYPE_OFFSET_BASETYPE (t), dn->complete);
  xml_print_type_attribute (xdi, TREE_TYPE (t), dn->complete);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, "/>\n");
}

/* Output for an ARRAY_TYPE.  */
static void
xml_output_array_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  fprintf (xdi->file, "  <ArrayType");
  xml_print_id_attribute (xdi, dn);
  xml_print_array_attributes (xdi, t);
  xml_print_type_attribute (xdi, TREE_TYPE (t), dn->complete);
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, "/>\n");
}

/* Output for an ENUMERAL_TYPE.  */
static void
xml_output_enumeral_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  tree tv;

  fprintf (xdi->file, "  <Enumeration");
  xml_print_id_attribute (xdi, dn);
  xml_print_name_attribute (xdi, DECL_NAME (TYPE_NAME (t)));
  xml_print_context_attribute (xdi, TYPE_NAME (t));
  xml_print_access_attribute (xdi, TYPE_NAME (t));
  xml_print_location_attribute (xdi, TYPE_NAME (t));
  xml_print_attributes_attribute (xdi, TYPE_ATTRIBUTES(t), 0);
  fprintf (xdi->file, ">\n");

  /* Output the list of possible values for the enumeration type.  */
  for (tv = TYPE_VALUES (t); tv ; tv = TREE_CHAIN (tv))
    {
    int value = TREE_INT_CST_LOW (TREE_VALUE (tv));
    fprintf (xdi->file,
             "    <EnumValue name=\"%s\" init=\"%d\"/>\n",
             xml_get_encoded_string ( TREE_PURPOSE(tv)), value);
    }

  fprintf (xdi->file, "  </Enumeration>\n");
}

/* Output for a cv-qualified type.  */
static void
xml_output_cv_qualified_type (xml_dump_info_p xdi, tree t, xml_dump_node_p dn)
{
  if (t == TYPE_MAIN_VARIANT(t))
    {
    switch (TREE_CODE(t))
      {
      case UNION_TYPE:
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

/* ------------------------------------------------------------------------ */

/* When a class, struct, or union type is defined, it is automatically
   given a member typedef to itself.  Given a RECORD_TYPE or
   UNION_TYPE, this returns that field's name.  Although this should
   never happen, 0 is returned when the field cannot be found.  */
static tree
xml_find_self_typedef_name (tree rt)
{
  tree field;
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
        if (!DECL_ARTIFICIAL(ts))
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
    for (tl = TYPE_FIELDS (TREE_TYPE (td)); tl; tl = TREE_CHAIN (tl))
      {
      if (TREE_CODE (tl) == TEMPLATE_DECL)
        {
        xml_add_template_decl (xdi, tl, 1);
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

/* Add tree node N to those encountered.  Return its index.  */
int
xml_add_node (xml_dump_info_p xdi, tree n, int complete)
{
  /* Some nodes don't need to be dumped and just refer to other nodes.
     These nodes should can have index zero because they should never
     be referenced.  */
  switch (TREE_CODE (n))
    {
    case NAMESPACE_DECL:
#if defined(XML_HAVE_FAKE_STD_NODE)
      if(n != fake_std_node)
#endif
        {
        return xml_add_node_real (xdi, n, complete);
        }
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

/* ------------------------------------------------------------------------ */

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
  if (TREE_CODE (scope) == NAMESPACE_DECL)
    {
    node = lookup_namespace_name (scope, id);
    }
  else if ((TREE_CODE (scope) == TYPE_DECL)
           && (TREE_CODE (TREE_TYPE (scope)) == RECORD_TYPE))
    {
    node = lookup_member (TREE_TYPE (scope), id, 2, 0);
    }

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

/* ------------------------------------------------------------------------ */

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
tree
xml_get_encoded_identifier_from_string (const char* in_str)
{
  int length_increase = 0;
  const char* inCh;
  char* outCh;
  char* newStr;
  tree id;

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
