/* XML output implementation for the C++ compiler.

   Brad King (brad.king@kitware.com)
   Kitware, Inc.   (www.kitware.com)

This file WILL HOPEFULLY BE part of GNU CC (eventually).

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


/* Organization of this source:
   The entry point is do_xml_output(), which is called from the end of
   finish_translation_unit() in semantics.c.
   
   xml_output_* functions are used to output tree nodes that require
   non-trivial processing (like RECORD_TYPE).  This includes any nodes
   whose XML elements have beginning and ending tags (not empty elements)
   and have other elements nested inside them.

   print_*_begin_tag functions are used to write out the XML begin tags
   with their attributes.  The functions are responsible for extracting the
   details from a tree node needed specifically for the XML attributes on
   the begin tag.

   print_*_end_tag functions print the corresponding XML end tags for
   each of the print_*_begin_tag functions.

   print_*_empty_tag functions print simple tree nodes whose XML elements
   are empty (have nothing nested inside them, and use the special <.../>
   syntax.  These elements may have attributes in the tags.
*/

#include "config.h"
#include "system.h"
#include "sys/stat.h"

#include "tree.h"
#include "cp-tree.h"
#include "decl.h"
#include "rtl.h"
#include "varray.h"

#include "splay-tree.h"

typedef struct xml_type_queue
{
  /* The queued type node.  */
  splay_tree_node node;

  /* The next node in the queue.  */
  struct xml_type_queue *next;
} *xml_type_queue_p;

typedef struct xml_file_queue
{
  /* The queued file name.  */
  splay_tree_node node;

  /* The next node in the queue.  */
  struct xml_file_queue *next;
} *xml_file_queue_p;

typedef struct xml_dump_info
{
  /* Output file stream of dump.  */
  FILE* file;

  /* Number of spaces to indent each nested element in XML output.  */
  unsigned int indent;


  /* Index of the next available type queue position.  */
  unsigned int type_index;

  /* The first type in the queue of types.  */
  xml_type_queue_p type_queue;

  /* The last type in the queue of types.  */
  xml_type_queue_p type_queue_end;

  /* All types that have been queued.  */
  splay_tree type_nodes;


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
/*#define DECL_INTERNAL_P(d) (strcmp (DECL_SOURCE_FILE (d), "<internal>")==0)*/

void do_xml_output                            PARAMS ((const char*));

static unsigned int xml_queue_type PARAMS((xml_dump_info_p, tree));
static unsigned int xml_queue_file PARAMS((xml_dump_info_p, const char*));

static void xml_output_headers                PARAMS ((xml_dump_info_p));
static void xml_output_namespace_decl         PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_type_decl              PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_record_type            PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_function_decl          PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_overload               PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_var_decl               PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_field_decl             PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_const_decl             PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_template_decl          PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_template_info          PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_typedef                PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_type                   PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_named_type             PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_void_type              PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_function_type          PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_method_type            PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_pointer_type           PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_reference_type         PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_offset_type            PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_array_type             PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_enumeral_type          PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_base_class             PARAMS ((xml_dump_info_p, unsigned long, tree));
static void xml_output_argument               PARAMS ((xml_dump_info_p, unsigned long, tree, tree));
static void xml_output_template_argument      PARAMS ((xml_dump_info_p, unsigned long, tree));

static void xml_dump_types PARAMS((xml_dump_info_p, unsigned long));
static void xml_dump_files PARAMS((xml_dump_info_p, unsigned long));

static const char* xml_get_qualified_name            PARAMS((tree t));

static const char* xml_get_encoded_string             PARAMS ((tree));
static const char* xml_get_encoded_string_from_string PARAMS ((const char*));
static tree xml_get_encoded_identifier_from_string PARAMS ((const char*));

static char* xml_concat_3_strings          PARAMS ((const char*, const char*, const char*));

static void print_indent                   PARAMS ((FILE*, unsigned long));
static tree reverse_opname_lookup          PARAMS ((tree));



/* Main XML output function.  Called by parser at the end of a translation
   unit.  Walk the entire translation unit starting at the global
   namespace.  Output all declarations.  */
void
do_xml_output (const char* filename)
{
  FILE *file;

  file = fopen (filename, "w");
  if (!file)
    cp_error ("could not open xml-dump file `%s'", filename);
  else
    {
    /* Prepare dump.  */
    struct xml_dump_info xdi;
    xdi.file = file;
    xdi.indent = 2;
    xdi.type_queue = 0;
    xdi.type_queue_end = 0;
    xdi.type_index = 0;
    xdi.type_nodes = splay_tree_new (splay_tree_compare_pointers, 0, 0);
    xdi.file_queue = 0;
    xdi.file_queue_end = 0;
    xdi.file_index = 0;
    xdi.file_nodes = splay_tree_new (splay_tree_compare_pointers, 0, 0);

    /* Do dump.  */
    xml_output_headers (&xdi);
    fprintf (file, "<GCC_XML>\n");
    xml_output_namespace_decl (&xdi, 0, global_namespace);
    fprintf (file, "<Types>\n");
    xml_dump_types(&xdi, xdi.indent);
    fprintf (file, "</Types>\n");
    fprintf (file, "<Files>\n");
    xml_dump_files(&xdi, xdi.indent);
    fprintf (file, "</Files>\n");
    fprintf (file, "</GCC_XML>\n");

    /* Clean up.  */
    splay_tree_delete (xdi.type_nodes);
    splay_tree_delete (xdi.file_nodes);
    fclose (file);
    }
}

/* Queue a type for later output.  If the type has already been queued, it
   is not queued again.  In either case, the queue index assigned to the
   type is returned.  */
unsigned int
xml_queue_type (xml_dump_info_p xdi, tree t)
{
  /* See if we've already queued this type.  */
  splay_tree_node n = splay_tree_lookup (xdi->type_nodes, (splay_tree_key) t);
  if (n)
    {
    /* Type was already queued.  Return its index.  */
    return n->value;
    }
  else
    {
    /* Type needs to be queued.  */
    xml_type_queue_p tq;

    /* Assign the next available index.  */
    unsigned int index = xdi->type_index++;

    /* Obtain a new queue node.  */
    tq = (xml_type_queue_p) xmalloc (sizeof (struct xml_type_queue));

    /* Create a new entry in the splay-tree.  */
    tq->node = splay_tree_insert (xdi->type_nodes, (splay_tree_key) t, 
                                  (splay_tree_value) index);
    
    /* Add it to the end of the queue.  */
    tq->next = 0;
    if (!xdi->type_queue_end)
      {
      xdi->type_queue = tq;
      }
    else
      {
      xdi->type_queue_end->next = tq;
      }
    xdi->type_queue_end = tq;
    
    /* Return the index.  */
    return index;
    }
}

/* Queue a filename for later output.  If the file has already been
   queued, it is not queued again.  In either case, the queue index
   assigned to the file is returned.  */
unsigned int
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
    fq->node = splay_tree_insert (xdi->file_nodes, (splay_tree_key) t, 
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

/* Print the XML attribute id="tid" for the given type. */
static void
print_id_attribute (xml_dump_info_p xdi, tree t)
{
  fprintf (xdi->file, " id=\"t%d\"", xml_queue_type (xdi, t));
}

/* Print the XML attribute extern="1" if the given decl is external. */
static void
print_extern_attribute (xml_dump_info_p xdi, tree d)
{
  if (DECL_EXTERNAL (d))
    {
    fprintf (xdi->file, " extern=\"1\"");
    }
}

/* Print the XML attribute extern="1" if the given decl is external. */
static void
print_function_extern_attribute (xml_dump_info_p xdi, tree fd)
{
  if (DECL_REALLY_EXTERN (fd))
    {
    fprintf (xdi->file, " extern=\"1\"");
    }
}

/* Print the XML attribute location="fid:line" for the given decl. */
static void
print_location_attribute (xml_dump_info_p xdi, tree d)
{
  unsigned int source_file = xml_queue_file (xdi, DECL_SOURCE_FILE (d));
  unsigned int source_line = DECL_SOURCE_LINE (d);
  
  fprintf (xdi->file, " location=\"f%d:%d\"", source_file, source_line);
}

/* Print the XML attribute type="tid" for the given type. */
static void
print_type_attribute (xml_dump_info_p xdi, tree t)
{
  unsigned int type = xml_queue_type (xdi, t);
  fprintf (xdi->file, " type=\"t%d\"", type);
}

/* Print the XML attribute name="..." for the given node. */
static void
print_name_attribute (xml_dump_info_p xdi, tree t)
{
  const char* name = xml_get_encoded_string (t);  
  fprintf (xdi->file, " name=\"%s\"", name);
}

/* Print the XML attribute returns="tid" for the given function type. */
static void
print_returns_attribute (xml_dump_info_p xdi, tree ft)
{
  unsigned int type = xml_queue_type (xdi, TREE_TYPE (ft));
  fprintf (xdi->file, " returns=\"t%d\"", type);
}

/* Print the XML attribute access="..." for the given decl. */
static void
print_access_attribute (xml_dump_info_p xdi, tree d)
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
    fprintf (xdi->file, " access=\"public\"");
    }
}

/* Print the XML attribute access="..." for the given binfo. */
static void
print_base_access_attribute (xml_dump_info_p xdi, tree binfo)
{
  if (TREE_VIA_PUBLIC (binfo))
    {
    fprintf (xdi->file, " access=\"public\"");
    }
  else if (TREE_VIA_PROTECTED (binfo))
    {
    fprintf (xdi->file, " access=\"protected\"");
    }
  else
    {
    fprintf (xdi->file, " access=\"private\"");
    }
}

/* Print the XML attribute incomplete="..." for the given type. */
static void
print_incomplete_attribute (xml_dump_info_p xdi, tree t)
{
  if (!COMPLETE_TYPE_P (t))
    {
    fprintf (xdi->file, " incomplete=\"1\"");
    }
}

/* Print the XML attribute abstract="..." for the given type. */
static void
print_abstract_attribute (xml_dump_info_p xdi, tree t)
{
  if (CLASSTYPE_PURE_VIRTUALS (t) != 0)
    {
    fprintf (xdi->file, " abstract=\"1\"");
    }
}

/* Print XML attribute init="..." for a variable initializer.  */
static void
print_init_attribute (xml_dump_info_p xdi, tree t)
{
  const char* value;
  
  if (!t || (t == error_mark_node)) return;
  
  value = xml_get_encoded_string_from_string (expr_as_string (t, 0));
  fprintf (xdi->file, " init=\"%s\"", value);
}

/* Print XML attributes describing the cv-qualifiers of a type.  */
static void
print_cv_attributes (xml_dump_info_p xdi, tree t)
{  
  if (CP_TYPE_CONST_P (t))
    {
    fprintf (xdi->file, " const=\"1\"");
    }
  if (CP_TYPE_VOLATILE_P (t))
    {
    fprintf (xdi->file, " volatile=\"1\"");
    }
  if (CP_TYPE_RESTRICT_P (t))
    {
    fprintf (xdi->file, " restrict=\"1\"");
    }
}

/* Print XML attribute const="1" for const methods.  */
static void
print_const_method_attribute (xml_dump_info_p xdi, tree fd)
{  
  if (DECL_CONST_MEMFUNC_P (fd))
    {
    fprintf (xdi->file, " const=\"1\"");
    }
}

/* Print XML attribute static="1" for static methods.  */
static void
print_static_method_attribute (xml_dump_info_p xdi, tree fd)
{
  if (!DECL_NONSTATIC_MEMBER_FUNCTION_P (fd))
    {
    fprintf (xdi->file, " static=\"1\"");
    }
}

/* Print XML attributes virtual="" and pure_virtual="" for a decl.  */
static void
print_virtual_method_attributes (xml_dump_info_p xdi, tree d)
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

/* Print XML attribute for a default argument.  */
static void
print_default_argument_attribute (xml_dump_info_p xdi, tree t)
{
  const char* value =
    xml_get_encoded_string_from_string (expr_as_string (t, 0));  
  fprintf (xdi->file, " default=\"%s\"", value);
}


/* Print XML attributes min="0" max="..." for an array type.  */
static void
print_array_attributes (xml_dump_info_p xdi, tree at)
{
  const char* length = "";
  
  if (TYPE_DOMAIN (at))
    length = xml_get_encoded_string_from_string (
      expr_as_string (TYPE_MAX_VALUE (TYPE_DOMAIN (at)), 0));

  fprintf (xdi->file, " min=\"0\" max=\"%s\">\n", length);
}

/* Print XML attribute basetype="..." with the given type.  */
static void
print_base_type_attribute (xml_dump_info_p xdi, tree t)
{
  fprintf (xdi->file, " basetype=\"%s\"", xml_get_qualified_name (t));
}

/* ------------------------------------------------------------------------ */

/* Print XML begin tag for a namespace element.  */
static void
print_namespace_begin_tag (xml_dump_info_p xdi, unsigned long indent, tree ns)
{
  print_indent (xdi->file, indent);
  if(ns == global_namespace)
    {
    fprintf (xdi->file, "<GlobalNamespace>\n");
    }
  else
    {
    const char* name = xml_get_encoded_string (DECL_NAME (ns));

    fprintf (xdi->file, "<Namespace name=\"%s\">\n", name);
    }
}


/* Print XML end tag for a namespace element.  */
static void
print_namespace_end_tag (xml_dump_info_p xdi, unsigned long indent, tree ns)
{
  print_indent (xdi->file, indent);
  if(ns == global_namespace)
    {
    fprintf (xdi->file, "</GlobalNamespace>\n");
    }
  else
    {
    fprintf (xdi->file, "</Namespace>\n");
    }
}


/* Print XML begin tag for a class, struct, or union element.  */
static void
print_class_begin_tag (xml_dump_info_p xdi, unsigned long indent, tree rt)
{
  print_indent (xdi->file, indent);
  if (TREE_CODE(rt) == RECORD_TYPE)
    {
    if (CLASSTYPE_DECLARED_CLASS (rt))
      {
      fprintf (xdi->file, "<Class");
      }
    else
      {
      fprintf (xdi->file, "<Struct");
      }
    }
  else
    {
    fprintf (xdi->file, "<Union");
    }

  print_name_attribute (xdi, DECL_NAME (TYPE_NAME (rt)));
  print_access_attribute (xdi, TYPE_NAME (rt));
  print_abstract_attribute (xdi, rt);
  print_incomplete_attribute (xdi, rt);
  print_location_attribute (xdi, TYPE_NAME (rt));
  fprintf (xdi->file, ">\n");
}


/* Print XML end tag for a class, struct, or union element.  */
static void
print_class_end_tag (xml_dump_info_p xdi, unsigned long indent, tree rt)
{
  print_indent (xdi->file, indent);
  if (TREE_CODE(rt) == RECORD_TYPE)
    {
    if (CLASSTYPE_DECLARED_CLASS (rt))
      {
      fprintf (xdi->file,
               "</Class>\n");
      }
    else
      {
      fprintf (xdi->file,
               "</Struct>\n");
      }
    }
  else
    {
      fprintf (xdi->file,
               "</Union>\n");
    }
}

/* Print XML empty tag for an ellipsis at the end of an argument list.  */
static void
print_ellipsis_empty_tag (xml_dump_info_p xdi, unsigned long indent)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "<Ellipsis/>\n");
}


/* Print XML begin tag for a template instantiation element.  */
static void
print_instantiation_begin_tag (xml_dump_info_p xdi, unsigned long indent)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "<Instantiation>\n");
}


/* Print XML end tag for a template instantiation element.  */
static void
print_instantiation_end_tag (xml_dump_info_p xdi, unsigned long indent)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "</Instantiation>\n");
}


/* Print XML begin tag for a template argument element.  */
static void
print_template_argument_begin_tag (xml_dump_info_p xdi, unsigned long indent)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "<TemplateArgument>\n");
}


/* Print XML end tag for a template argument element.  */
static void
print_template_argument_end_tag (xml_dump_info_p xdi, unsigned long indent)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "</TemplateArgument>\n");
}

/* Print XML empty tag for a scope reference element (SCOPE_REF).  */
static void
print_scope_ref_empty_tag (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  const char* class_name =
    xml_get_encoded_string (DECL_NAME (TYPE_NAME (TREE_OPERAND (t, 0))));
  const char* field_name =
    xml_get_encoded_string (TREE_OPERAND (t, 1));
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "<ScopeRef class=\"%s\" field=\"%s\"/>\n",
           class_name, field_name);
}


/* Print XML empty tag for an integer literal element (INTEGER_CST).  */
static void
print_integer_cst_empty_tag (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<Integer value=\"");
  if (TREE_INT_CST_HIGH (t) == 0)
    {
    fprintf (xdi->file, HOST_WIDE_INT_PRINT_UNSIGNED, TREE_INT_CST_LOW (t));
    }
  else if ((TREE_INT_CST_HIGH (t) == -1) && (TREE_INT_CST_LOW (t) != 0))
    {
    fprintf (xdi->file, "-");
    fprintf (xdi->file, HOST_WIDE_INT_PRINT_UNSIGNED, -TREE_INT_CST_LOW (t));
    }
  else
    {
    fprintf (xdi->file, HOST_WIDE_INT_PRINT_DOUBLE_HEX,
             TREE_INT_CST_HIGH (t), TREE_INT_CST_LOW (t));
    }
  fprintf (xdi->file, "\"/>\n");
}


/* Print XML empty tag describing an unimplemented TREE_CODE that has been
   encountered.  */
static void
print_unimplemented_empty_tag (xml_dump_info_p xdi, unsigned long indent, tree t,
                               const char* func)
{
  int tree_code = TREE_CODE (t);
  
  print_indent (xdi->file, indent);
  fprintf (xdi->file,
           "<Unimplemented tree_code=\"%d\" tree_code_name=\"%s\""
           " xml_c_location=\"%s\"/>\n",
           tree_code, tree_code_name [tree_code], func);
}


/* Output the XML file's headers.  */
void
xml_output_headers (xml_dump_info_p xdi)
{
  fprintf (xdi->file,
           "<?xml version=\"1.0\"?>\n");/* encoding="ISO-8859-1"?> */
}


/* Output a NAMESPACE_DECL.  Prints beginning and ending tags, and all
   the namespace's member declarations in between.  */
void
xml_output_namespace_decl (xml_dump_info_p xdi, unsigned long indent, tree ns)
{
  if(ns == fake_std_node) return;
    
  /* Only walk a real namespace.  */
  if (!DECL_NAMESPACE_ALIAS (ns))
    {
    tree cur_decl;

    print_namespace_begin_tag(xdi, indent, ns);

    for (cur_decl = cp_namespace_decls(ns); cur_decl;
         cur_decl = TREE_CHAIN (cur_decl))
      {
      switch (TREE_CODE (cur_decl))
        {
        case NAMESPACE_DECL:
          xml_output_namespace_decl(xdi, indent+xdi->indent, cur_decl);
          break;
        case TYPE_DECL:
          xml_output_type_decl(xdi, indent+xdi->indent, cur_decl);
          break;          
        case FUNCTION_DECL:
          xml_output_function_decl (xdi, indent+xdi->indent, cur_decl);
          break;
        case VAR_DECL:
          xml_output_var_decl (xdi, indent+xdi->indent, cur_decl);
          break;
        case TREE_LIST:
          xml_output_overload (xdi, indent+xdi->indent,
                               TREE_VALUE (cur_decl));
          break;
        case CONST_DECL:
          xml_output_const_decl (xdi, indent+xdi->indent, cur_decl);
          break;
        case TEMPLATE_DECL:
          xml_output_template_decl (xdi, indent+xdi->indent, cur_decl);
          break;
        case RESULT_DECL:
        case USING_DECL:
          /* case THUNK_DECL: */
          /* This is compiler-generated.  Just ignore it.  */
          break;
        default:
          print_unimplemented_empty_tag (xdi, indent+xdi->indent,
                                         cur_decl, "xml_output_namespace_decl");
        }
      }
    
    print_namespace_end_tag(xdi, indent, ns);
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

    print_indent (xdi->file, indent);
    fprintf (xdi->file, "<NamespaceAlias");
    print_name_attribute (xdi, DECL_NAME (ns));
    fprintf (xdi->file, " namespace=\"%s\"", xml_get_qualified_name (real_ns));
    fprintf (xdi->file, "/>\n");
    }
}

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


/* Dispatch output of a TYPE_DECL.  This is either a typedef, or a new
   type (class/struct/union) definition.  */
void
xml_output_type_decl (xml_dump_info_p xdi, unsigned long indent, tree td)
{
  /* Get the type from the TYPE_DECL.  We don't want to use complete_type
     because it may modify something.  We are doing a read-only dump.  */
  tree t = TREE_TYPE (td);

  /* Don't process any internally generated declarations.  */
  if (DECL_INTERNAL_P (td)) return;

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
      xml_output_typedef (xdi, indent, td);
      break;
    case ENUMERAL_TYPE:
      if (DECL_ORIGINAL_TYPE (td))
        {
        /* A typedef to an existing enumeral type.  */
        xml_output_typedef (xdi, indent, td);
        }
      else
        {
        /* This is the declaration of a new enumeration type.  */
        xml_output_enumeral_type (xdi, indent, t);
        }
      break;
    case UNION_TYPE:
    case RECORD_TYPE:
      if ((TREE_CODE (t) == RECORD_TYPE) && TYPE_PTRMEMFUNC_P (t))
        {
        /* A typedef to a pointer to member.  */
        xml_output_typedef (xdi, indent, td);
        }
      /* This is a complicated test to tell apart a real class declaration
         or definition from a typedef to it.  This must be able to detect
         the difference for the following cases:

            Code:                           Meaning:
         1.  struct A {};                    "struct A"
         2.  struct A {}; typedef A B;       "struct A" "typedef A B"
         3.  typedef struct {} A;            "struct A"
         4.  typedef struct A {} A;          "struct A"
         5.  typedef struct A {} B;          "struct A" "typedef A B"

         DECL_IMPLICIT_TYPEDEF_P will recognize type TYPE_DECL that
         points to the real class definition for cases 1, 2, and 5,
         and is sufficient to make the decision.

         For cases 3 and 4, the second test makes sure the context of
         the TYPE_DECL matches the context of the RECORD_TYPE, which
         can only happen for a TYPE_DECL in the same scope as the
         class.  It then looks at the RECORD_TYPE to find its
         artificial member that is a typedef to its own class.  The
         name of this field is compared to the name of the original
         TYPE_DECL.  If it matches, then it is assumed that the
         TYPE_DECL corresponds to the class's definition.  Otherwise,
         it is assumed that it is simply a typedef.  */
      else if (DECL_IMPLICIT_TYPEDEF_P (td)
               || (((DECL_CONTEXT (td) == TYPE_CONTEXT (t))
                    && (xml_find_self_typedef_name (t) == DECL_NAME (td)))))
        {
        /* A new type definition.  */
        xml_output_record_type (xdi, indent, t);
        }
      else
        {
        /* A typedef to an existing class, struct, or union type.  */
        xml_output_typedef (xdi, indent, td);
        }
      break;
    default:
      print_unimplemented_empty_tag (xdi, indent, t,
                                     "xml_output_type_decl");
    }
}


/* Output a RECORD_TYPE that is not a pointer-to-member-function.  Prints
   beginning and ending tags, and all class member declarations between.
   Also handles a UNION_TYPE.  */
void
xml_output_record_type (xml_dump_info_p xdi, unsigned long indent, tree rt)
{
  tree field;
  tree func;
  int destructorFound = 0;

  print_class_begin_tag (xdi, indent, rt);
  
  if (CLASSTYPE_TEMPLATE_INFO (rt))
    {
    xml_output_template_info (xdi, indent+xdi->indent,
                              CLASSTYPE_TEMPLATE_INFO (rt));
    }  
  
  /* Output all the non-method declarations in the class.  */
  for (field = TYPE_FIELDS (rt) ; field ; field = TREE_CHAIN (field))
    {
    switch(TREE_CODE(field))
      {
      case TYPE_DECL:
        if (TREE_TYPE (field) == rt)
          {
          /* A class or struct internally typedefs itself.  */
          xml_output_typedef (xdi, indent+xdi->indent, field);
          }
        else
          {
          /* A nested type declaration.  */
          xml_output_type_decl (xdi, indent+xdi->indent, field);
          }
        break;          
      case FIELD_DECL:
        xml_output_field_decl (xdi, indent+xdi->indent, field);
        break;          
      case VAR_DECL:
        xml_output_var_decl (xdi, indent+xdi->indent, field);
        break;
      case CONST_DECL:
        xml_output_const_decl (xdi, indent+xdi->indent, field);
        break;
      case TEMPLATE_DECL:
        xml_output_template_decl (xdi, indent+xdi->indent, field);
        break;
      case USING_DECL:
        /* Ignore the using decl.  */
        break;
      case RESULT_DECL:
      default:
        print_unimplemented_empty_tag (xdi, indent+xdi->indent, field,
                                       "xml_output_record_type fields");
      }
    }

  /* Output all the method declarations in the class.  */
  for (func = TYPE_METHODS (rt) ; func ; func = TREE_CHAIN (func))
    {
    switch (TREE_CODE (func))
      {
      case FUNCTION_DECL:
        if (DECL_DESTRUCTOR_P (func)) 
          destructorFound = 1;
        xml_output_function_decl (xdi, indent+xdi->indent, func);
        break;
      case TEMPLATE_DECL:
        xml_output_template_decl (xdi, indent+xdi->indent, func);
        break;
      default:
        print_unimplemented_empty_tag (xdi, indent+xdi->indent, func,
                                       "xml_output_record_type methods");
      }
    }

  /* If no destructor was encountered, then the class has a trivial destructor.
     For some reason, the implicit declaration was not there, so we catch
     the case here.  */
  if(!destructorFound && TYPE_HAS_TRIVIAL_DESTRUCTOR (rt))
    {
    print_indent (xdi->file, indent+xdi->indent);
    fprintf (xdi->file, "<Destructor access=\"public\"/>\n");
    }
  
  /* Output all the base classes.  */
  {
  tree binfo = TYPE_BINFO (rt);
  tree binfos = BINFO_BASETYPES (binfo);
  int n_baselinks = binfos? TREE_VEC_LENGTH (binfos) : 0;
  int i;

  for (i = 0; i < n_baselinks; i++)
    {
    tree base_binfo = TREE_VEC_ELT (binfos, i);
    if (base_binfo)
      {
      /* Output this base class.  */
      xml_output_base_class (xdi, indent+xdi->indent, base_binfo);
      }
    }
  }
  
  print_class_end_tag (xdi, indent, rt);
}


/* Output for a FUNCTION_DECL.  Prints beginning and ending tags, and
   the argument list between them.  */
void
xml_output_function_decl (xml_dump_info_p xdi, unsigned long indent, tree fd)
{
  tree arg;
  tree arg_type;
  const char* tag;
  tree name = DECL_NAME (fd);
  int do_returns = 0;
  int do_access = 0;
  int do_const = 0;
  int do_virtual = 0;
  int do_static = 0;

  /* Don't process any internally generated declarations.  */
  if (DECL_INTERNAL_P (fd)) return;

  /* Don't process any compiler-generated functions except constructors
     and destructors.  */
  if (DECL_ARTIFICIAL(fd)
      && !DECL_CONSTRUCTOR_P (fd)
      && !DECL_DESTRUCTOR_P (fd)) return;

  /* Don't output the cloned functions.  */
  if (DECL_CLONED_FUNCTION_P (fd)) return;

  /* Print out the begin tag for this type of function.  */
  if (DECL_CONSTRUCTOR_P (fd))
    {
    /* A class constructor.  */
    tag = "Constructor"; do_access = 1;
    }
  else if (DECL_DESTRUCTOR_P (fd))
    {
    /* A class destructor.  */
    tag = "Destructor"; do_access = 1; do_virtual = 1;
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
        name = reverse_opname_lookup (DECL_NAME (fd));
        do_returns = 1; do_access = 1; do_const = 1; do_virtual = 1;
        do_static = 1;
        }
      else
        {
        /* An operator in a namespace.  */
        tag = "OperatorFunction";
        name = reverse_opname_lookup (DECL_NAME (fd));
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

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<%s", tag);
  print_name_attribute (xdi, name);
  if(do_returns) print_returns_attribute (xdi, TREE_TYPE (fd));
  if(do_access)  print_access_attribute (xdi, fd);
  if(do_const)   print_const_method_attribute (xdi, fd);
  if(do_virtual) print_virtual_method_attributes (xdi, fd);
  if(do_static)  print_static_method_attribute (xdi, fd);
  print_location_attribute (xdi, fd);
  print_function_extern_attribute (xdi, fd);
  fprintf (xdi->file, ">\n");

  if (DECL_TEMPLATE_INFO (fd))
    {
    xml_output_template_info (xdi, indent+xdi->indent,
                              DECL_TEMPLATE_INFO (fd));
    }
  
  /* Prepare to iterator through argument list.  */
  arg = DECL_ARGUMENTS (fd);
  arg_type = TYPE_ARG_TYPES (TREE_TYPE (fd));
  if (DECL_NONSTATIC_MEMBER_FUNCTION_P (fd))
    {
    /* Skip "this" argument.  */
    if(arg) arg = TREE_CHAIN (arg);
    arg_type = TREE_CHAIN (arg_type);
    }  
  
  /* Print out the argument list for this function.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (xdi, indent+xdi->indent, arg, arg_type);
    if(arg) arg = TREE_CHAIN (arg);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(!arg_type)
    {
    /* Function has variable number of arguments.  */
    print_ellipsis_empty_tag(xdi, indent+xdi->indent);
    }

  /* Print out the end tag for this type of function.  */
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "</%s>\n", tag);
}


/* Output for an OVERLOAD.  Output all the functions in the overload list.   */
void
xml_output_overload (xml_dump_info_p xdi, unsigned long indent, tree o)
{
  tree cur_overload = o;
  while (cur_overload)
    {
    xml_output_function_decl (xdi, indent, OVL_CURRENT (cur_overload));
    cur_overload = OVL_NEXT (cur_overload);
    }
}


/* Output for a VAR_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
void
xml_output_var_decl (xml_dump_info_p xdi, unsigned long indent, tree vd)
{
  /* Don't process any internally generated declarations.  */
  if (DECL_INTERNAL_P (vd)) return;

  print_indent (xdi->file, indent);  
  fprintf (xdi->file, "<Variable");
  print_name_attribute (xdi, DECL_NAME (vd));
  print_type_attribute (xdi, TREE_TYPE (vd));
  print_init_attribute (xdi, DECL_INITIAL (vd));
  print_location_attribute (xdi, vd);
  print_extern_attribute (xdi, vd);
  fprintf (xdi->file, "/>\n");
}


/* Output for a FIELD_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
void
xml_output_field_decl (xml_dump_info_p xdi, unsigned long indent, tree fd)
{
  /* Don't process any internally generated declarations.  */
  if (DECL_INTERNAL_P (fd)) return;
  
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<Field");
  print_name_attribute (xdi, DECL_NAME (fd));
  /* TODO: handle bit field case.  */
  print_type_attribute (xdi, TREE_TYPE (fd));
  print_location_attribute (xdi, fd);
  fprintf (xdi->file, "/>\n");
}


/* Output for a CONST_DECL.  The name and type of the constant are output,
   as well as the value.  */
void
xml_output_const_decl (xml_dump_info_p xdi, unsigned long indent, tree cd)
{
  /* Don't process any internally generated declarations.  */
  if (DECL_INTERNAL_P (cd)) return;

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<Enum");
  print_name_attribute (xdi, DECL_NAME (cd));
  print_type_attribute (xdi, TREE_TYPE (cd));
  print_init_attribute (xdi, DECL_INITIAL (cd));
  print_location_attribute (xdi, cd);
  fprintf (xdi->file, "/>\n");
}


/* Output for a TEMPLATE_DECL.  The set of specializations (including
   instantiations) is output.  */
void
xml_output_template_decl (xml_dump_info_p xdi, unsigned long indent, tree td)
{
  tree tl;
  
  /* Don't process any internally generated declarations.  */
  if (DECL_INTERNAL_P (td)) return;

  for (tl = DECL_TEMPLATE_SPECIALIZATIONS (td);
       tl ; tl = TREE_CHAIN (tl))
    {
    tree ts = TREE_VALUE (tl);
    switch (TREE_CODE (ts))
      {
      case FUNCTION_DECL:
        xml_output_function_decl (xdi, indent, ts);
        break;
      case TEMPLATE_DECL:
        break;
      default:
        print_unimplemented_empty_tag (xdi, indent, ts,
                                       "xml_output_template_decl SPECIALIZATIONS");
      }
    }

  for (tl = DECL_TEMPLATE_INSTANTIATIONS (td);
       tl ; tl = TREE_CHAIN (tl))
    {
    tree ts = TYPE_NAME (TREE_VALUE (tl));
    switch (TREE_CODE (ts))
      {
      case TYPE_DECL:
        xml_output_type_decl (xdi, indent, ts);
        break;
      default:
        print_unimplemented_empty_tag (xdi, indent, ts,
                                       "xml_output_template_decl INSTANTIATIONS");
      }
    }
}


/* Output for TEMPLATE_INFO.  The set of template parameters used
   is output.  */
void
xml_output_template_info (xml_dump_info_p xdi, unsigned long indent, tree ti)
{
  tree arg_vec;
  int num_args;
  int i;

  print_instantiation_begin_tag (xdi, indent);

  arg_vec = TI_ARGS (ti);
  num_args = TREE_VEC_LENGTH (arg_vec);
  for (i=0 ; i < num_args ; ++i)
    {
    tree arg = TREE_VEC_ELT (arg_vec, i);
    xml_output_template_argument (xdi, indent+xdi->indent, arg);
    }

  print_instantiation_end_tag (xdi, indent);  
}


/* Output for a typedef.  The name and associated type are output.  */
void
xml_output_typedef (xml_dump_info_p xdi, unsigned long indent, tree td)
{
  /* If the typedef points to its own name in the same context, ignore
     it.  This can happen for code like "typedef struct {} A;".  */
  if(!DECL_ORIGINAL_TYPE (td)
     && TREE_TYPE (td) && TYPE_NAME (TREE_TYPE (td))
     && (DECL_NAME (td) == DECL_NAME (TYPE_NAME (TREE_TYPE (td))))
     && (DECL_CONTEXT (td) == DECL_CONTEXT (TYPE_NAME (TREE_TYPE (td)))))
     return;

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<Typedef");
  print_name_attribute (xdi, DECL_NAME (td));

  /* Get the original type out of the typedef, if any.  */
  if (DECL_ORIGINAL_TYPE (td))
    print_type_attribute (xdi, DECL_ORIGINAL_TYPE (td));
  else
    print_type_attribute (xdi, TREE_TYPE (td));

  print_location_attribute (xdi, td);
  fprintf (xdi->file, "/>\n");
}

/* Output for a *_TYPE.  */
void
xml_output_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  switch (TREE_CODE (t))
    {
    case ARRAY_TYPE:
      xml_output_array_type (xdi, indent, t);
      break;
    case POINTER_TYPE:
      xml_output_pointer_type (xdi, indent, t);
      break;
    case REFERENCE_TYPE:
      xml_output_reference_type (xdi, indent, t);
      break;
    case FUNCTION_TYPE:
      xml_output_function_type (xdi, indent, t);
      break;
    case METHOD_TYPE:
      xml_output_method_type (xdi, indent, t);
      break;
    case OFFSET_TYPE:
      xml_output_offset_type (xdi, indent, t);
      break;
    case RECORD_TYPE:
      if (TYPE_PTRMEMFUNC_P (t))
        {
        /* Pointer-to-member-functions are stored ina RECORD_TYPE.  */
        xml_output_type (xdi, indent, TYPE_PTRMEMFUNC_FN_TYPE (t));
        }
      else
        {
        /* This is a struct or class type, just output its name.  */
        xml_output_named_type (xdi, indent, t);
        }
      break;
    case LANG_TYPE:
    case INTEGER_TYPE:
    case BOOLEAN_TYPE:
    case REAL_TYPE:
    case COMPLEX_TYPE:
    case ENUMERAL_TYPE:
    case UNION_TYPE:
    case TYPENAME_TYPE:
    case TEMPLATE_TYPE_PARM:
      xml_output_named_type (xdi, indent, t);
      break;
    case SCOPE_REF:
      print_scope_ref_empty_tag (xdi, indent, t);
      break;
    case TEMPLATE_PARM_INDEX:
      xml_output_type (xdi, indent, TREE_TYPE (t));
      break;
    case VOID_TYPE:
      xml_output_void_type (xdi, indent, t);
      break;
    case INTEGER_CST:
      print_integer_cst_empty_tag (xdi, indent, t);
      break;
    default:
      print_unimplemented_empty_tag (xdi, indent, t, "xml_output_type");
    }
}


/* Output for a normal named type.  */
void
xml_output_named_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<NamedType");
  print_id_attribute (xdi, t);
  fprintf (xdi->file, " name=\"%s\"", xml_get_qualified_name (t));
  print_cv_attributes (xdi, t);
  fprintf (xdi->file, "/>\n");
}


/* Output for a void type.  */
void
xml_output_void_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<NamedType");
  print_id_attribute (xdi, t);
  fprintf (xdi->file, " name=\"%s\"",
           xml_get_qualified_name (TYPE_MAIN_VARIANT (t)));
  print_cv_attributes (xdi, t);
  fprintf (xdi->file, "/>\n");
}


/* Output for a FUNCTION_TYPE.  */
void
xml_output_function_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  tree arg_type;
  
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<FunctionType");
  print_id_attribute (xdi, t);
  print_returns_attribute (xdi, t);
  print_cv_attributes (xdi, t);
  fprintf (xdi->file, ">\n");
  
  /* Prepare to iterator through argument list.  */
  arg_type = TYPE_ARG_TYPES (t);

  /* Print out the argument list for this function.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (xdi, indent+xdi->indent, NULL, arg_type);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(arg_type != void_list_node)
    {
    /* Function has variable number of arguments.  */
    print_ellipsis_empty_tag(xdi, indent+xdi->indent);
    }

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "</FunctionType>\n");
}


/* Output for a METHOD_TYPE.  */
void
xml_output_method_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  tree arg_type;
  
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<MethodType");
  print_id_attribute (xdi, t);
  print_base_type_attribute (xdi, TYPE_METHOD_BASETYPE (t));
  print_returns_attribute (xdi, t);
  print_cv_attributes (xdi, t);
  fprintf (xdi->file, ">\n");
  
  /* Prepare to iterator through argument list.  */
  arg_type = TYPE_ARG_TYPES (t);

  /* Skip "this" argument.  */
  arg_type = TREE_CHAIN (arg_type);

  /* Print out the argument list for this method.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (xdi, indent+xdi->indent, NULL, arg_type);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(arg_type != void_list_node)
    {
    /* Method has variable number of arguments.  */
    print_ellipsis_empty_tag(xdi, indent+xdi->indent);
    }

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "</MethodType>\n");
}


/* Output for a POINTER_TYPE.  */
void
xml_output_pointer_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<PointerType");
  print_id_attribute (xdi, t);
  print_cv_attributes (xdi, t);
  print_type_attribute (xdi, TREE_TYPE (t));
  fprintf (xdi->file, "/>\n");
}


/* Output for a REFERENCE_TYPE.  */
void
xml_output_reference_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<ReferenceType");
  print_id_attribute (xdi, t);
  print_cv_attributes (xdi, t);
  print_type_attribute (xdi, TREE_TYPE (t));
  fprintf (xdi->file, "/>\n");
}


/* Output for an OFFSET_TYPE.  */
void
xml_output_offset_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<OffsetType");
  print_id_attribute (xdi, t);
  print_base_type_attribute (xdi, TYPE_OFFSET_BASETYPE (t));
  print_cv_attributes (xdi, t);
  print_type_attribute (xdi, TREE_TYPE (t));
  fprintf (xdi->file, "/>\n");
}


/* Output for an ARRAY_TYPE.  */
void
xml_output_array_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<ArrayType");
  print_id_attribute(xdi, t);
  print_array_attributes (xdi, t);
  print_cv_attributes (xdi, t);
  print_type_attribute (xdi, TREE_TYPE (t));
  fprintf (xdi->file, "/>\n");
}


/* Output for an ENUMERAL_TYPE.  */
void
xml_output_enumeral_type (xml_dump_info_p xdi, unsigned long indent, tree t)
{
  tree tv;

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<Enumeration");
  print_name_attribute (xdi, DECL_NAME (TYPE_NAME (t)));
  print_access_attribute (xdi, TYPE_NAME (t));
  print_location_attribute (xdi, TYPE_NAME (t));
  fprintf (xdi->file, ">\n");

  /* Output the list of possible values for the enumeration type.  */
  for (tv = TYPE_VALUES (t); tv ; tv = TREE_CHAIN (tv))
    {
    print_indent (xdi->file, indent+xdi->indent);
    fprintf (xdi->file,
             "<EnumValue name=\"%s\"/>\n",
             xml_get_encoded_string ( TREE_PURPOSE(tv)));
    }

  print_indent (xdi->file, indent);
  fprintf (xdi->file, "</Enumeration>\n");
}


/* Output a base class for a RECORD_TYPE or UNION_TYPE.  */
void
xml_output_base_class (xml_dump_info_p xdi, unsigned long indent, tree binfo)
{
  print_indent (xdi->file, indent);
  fprintf (xdi->file, "<BaseClass");
  print_base_access_attribute (xdi, binfo);
  fprintf (xdi->file, " name=\"%s\"",
           xml_get_qualified_name (BINFO_TYPE (binfo)));
  fprintf (xdi->file, "/>\n");
}


/* Output for a PARM_DECL / TREE_LIST corresponding to a function argument.  */
void
xml_output_argument (xml_dump_info_p xdi, unsigned long indent, tree pd, tree tl)
{
  /* Don't process any compiler-generated arguments.  These occur for
     things like constructors of classes with virtual inheritance.  */
  if (pd && DECL_ARTIFICIAL (pd)) return;

  print_indent (xdi->file, indent);  
  fprintf (xdi->file, "<Argument");
  if(pd && DECL_NAME (pd))
    {
    print_name_attribute (xdi, DECL_NAME (pd));
    }

  if (pd && DECL_ARG_TYPE_AS_WRITTEN (pd))
    {
    print_type_attribute (xdi, DECL_ARG_TYPE_AS_WRITTEN (pd));
    }
  else if (pd && TREE_TYPE (pd))
    {
    print_type_attribute (xdi, TREE_TYPE (pd));
    }
  else
    {
    print_type_attribute (xdi, TREE_VALUE (tl));
    }
  
  if (TREE_PURPOSE (tl))
    {
    print_default_argument_attribute (xdi, TREE_PURPOSE (tl));
    }

  fprintf (xdi->file, "/>\n");
}


/* Output the given template argument.  */
void
xml_output_template_argument (xml_dump_info_p xdi, unsigned long indent, tree arg)
{
  print_template_argument_begin_tag (xdi, indent);
  xml_output_type (xdi, indent + xdi->indent, arg);
  print_template_argument_end_tag (xdi, indent);
}

/* Dump the queue of types.  */
void xml_dump_types (xml_dump_info_p xdi, unsigned long indent)
{
  xml_type_queue_p tq;
  xml_type_queue_p next_tq; 
  for(tq = xdi->type_queue; tq ; tq = next_tq)
    {
    xml_output_type (xdi, indent, (tree) tq->node->key);
    next_tq = tq->next;
    free (tq);
    }
}

/* Dump the queue of file names.  */
void xml_dump_files (xml_dump_info_p xdi, unsigned long indent)
{
  xml_file_queue_p fq;
  xml_file_queue_p next_fq; 
  for(fq = xdi->file_queue; fq ; fq = next_fq)
    {
    print_indent (xdi->file, indent);
    fprintf (xdi->file, "<File id=\"f%d\" name=\"%s\"/>\n",
             (unsigned int) fq->node->value,
             IDENTIFIER_POINTER ((tree) fq->node->key));
    next_fq = fq->next;
    free (fq);
    }
}

/* Given any _DECL, return a string with the fully qualified form of
   its name.  */
const char*
xml_get_qualified_name (tree t)
{
  tree context;
  tree qualifiers = NULL_TREE;
  tree n;
  
  /* Build a list of all the qualifying contexts out to the global
     namespace.  */
  context = t;
  while (context && (context != global_namespace))
    {
    if (TYPE_P (context)) context = TYPE_NAME (context);

    qualifiers = tree_cons (NULL_TREE, context, qualifiers);

    context = CP_DECL_CONTEXT (context);
    }
  
  qualifiers = nreverse(qualifiers);
  if(qualifiers)
    {
    char* temp_name;
    tree q = qualifiers;
    char* name =
      strdup (IDENTIFIER_POINTER (DECL_NAME (TREE_VALUE (q))));
    for (q = TREE_CHAIN(q); q ; q = TREE_CHAIN (q))
      {
      temp_name = xml_concat_3_strings (
        IDENTIFIER_POINTER (DECL_NAME (TREE_VALUE (q))), "::", name);
      free (name);
      name = temp_name;
      }
    n = get_identifier (name);
    free (name);
    }
  else
    {
    n = get_identifier("");
    }
  return IDENTIFIER_POINTER (n);
}

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


/* Concatenate input strings into new memory.  Caller must free memory.  */
char*
xml_concat_3_strings (const char* s1, const char* s2, const char* s3)
{
  char* ns = (char*) malloc (strlen (s1) +  strlen (s2) + strlen (s3) + 1);
  strcpy (ns, s1);
  strcat (ns, s2);
  strcat (ns, s3);
  return ns;
}

/* Print N spaces to FILE.  Used for indentation of XML output.  */
void
print_indent (FILE* file, unsigned long n)
{
  unsigned long left = n;
  while(left >= 10)
    {
    fprintf (file, "          ");
    left -= 10;
    }
  while(left > 0)
    {
    fprintf (file, " ");
    left -= 1;
    }
}


/* Lookup the real name of an operator whose ansi_opname or ansi_assopname
   is NAME.  */
tree
reverse_opname_lookup (tree name)
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

