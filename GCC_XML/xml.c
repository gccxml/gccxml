#include "config.h"
#include "system.h"
#include "sys/stat.h"

#include "tree.h"
#include "cp-tree.h"
#include "decl.h"
#include "rtl.h"
#include "varray.h"

#define MY_TYPEDEF_TEST(d) \
  (DECL_NAME ((d)) != DECL_NAME (TYPE_NAME (TREE_TYPE ((d)))))

#define MY_DECL_INTERNAL(d) (strcmp (DECL_SOURCE_FILE (d), "<internal>")==0)
//#define MY_DECL_INTERNAL(d) 0

/* Useful debug macro.  Will be removed.  */
#define DEBUG_LINE(f) \
  fprintf ((f), "%s: %d\n", __FILE__, __LINE__)


void do_xml_output                            PARAMS ((void));
    
static void xml_output_headers                PARAMS ((FILE*));
static void xml_output_namespace_decl         PARAMS ((FILE*, unsigned long, tree));
static void xml_output_type_decl              PARAMS ((FILE*, unsigned long, tree));
static void xml_output_record_type            PARAMS ((FILE*, unsigned long, tree));
static void xml_output_function_decl          PARAMS ((FILE*, unsigned long, tree));
static void xml_output_overload               PARAMS ((FILE*, unsigned long, tree));
static void xml_output_var_decl               PARAMS ((FILE*, unsigned long, tree));
static void xml_output_field_decl             PARAMS ((FILE*, unsigned long, tree));
static void xml_output_const_decl             PARAMS ((FILE*, unsigned long, tree));
static void xml_output_template_decl          PARAMS ((FILE*, unsigned long, tree));
static void xml_output_template_info          PARAMS ((FILE*, unsigned long, tree));
static void xml_output_typedef                PARAMS ((FILE*, unsigned long, tree));
static void xml_output_type                   PARAMS ((FILE*, unsigned long, tree));
static void xml_output_named_type             PARAMS ((FILE*, unsigned long, tree));
static void xml_output_function_type          PARAMS ((FILE*, unsigned long, tree));
static void xml_output_method_type            PARAMS ((FILE*, unsigned long, tree));
static void xml_output_pointer_type           PARAMS ((FILE*, unsigned long, tree));
static void xml_output_reference_type         PARAMS ((FILE*, unsigned long, tree));
static void xml_output_offset_type            PARAMS ((FILE*, unsigned long, tree));
static void xml_output_array_type             PARAMS ((FILE*, unsigned long, tree));
static void xml_output_base_class             PARAMS ((FILE*, unsigned long, tree));
static void xml_output_base_type              PARAMS ((FILE*, unsigned long, tree));
static void xml_output_argument               PARAMS ((FILE*, unsigned long, tree, tree));
static void xml_output_returns                PARAMS ((FILE*, unsigned long, tree));
static void xml_output_template_argument      PARAMS ((FILE*, unsigned long, tree));
static void xml_output_qualified_name         PARAMS ((FILE*, unsigned long, tree));

static char* xml_get_encoded_string             PARAMS ((tree));
static char* xml_get_encoded_string_from_string PARAMS ((const char*));

static void print_indent                   PARAMS ((FILE*, unsigned long));
static tree reverse_opname_lookup          PARAMS ((tree));

/* Number of spaces to indent each nested element in XML output.  */
#define XML_NESTED_INDENT 2



/* Main XML output function.  Called by parser at the end of a translation
   unit.  Walk the entire translation unit starting at the global
   namespace.  Output all declarations.  */
void
do_xml_output (void)
{
  xml_output_headers (stdout);
  xml_output_namespace_decl (stdout, 0, global_namespace);
}



/* Print XML begin tag for a namespace element.  */
static void
print_namespace_begin_tag (file, indent, ns)
  FILE* file;
  unsigned long indent;
  tree ns;
{
  print_indent (file, indent);
  if(ns == global_namespace)
    {
    fprintf (file, "<GlobalNamespace>\n");
    }
  else
    {
    char* name = xml_get_encoded_string (DECL_NAME (ns));

    fprintf (file, "<Namespace name=\"%s\">\n", name);
    }
}


/* Print XML end tag for a namespace element.  */
static void
print_namespace_end_tag (file, indent, ns)
  FILE* file;
  unsigned long indent;
  tree ns;
{
  print_indent (file, indent);
  if(ns == global_namespace)
    {
    fprintf (file, "</GlobalNamespace>\n");
    }
  else
    {
    fprintf (file, "</Namespace>\n");
    }
}


/* Print XML begin tag for a namespace alias element.  */
static void
print_namespace_alias_begin_tag (file, indent, ns)
  FILE* file;
  unsigned long indent;
  tree ns;
{
  char* name = xml_get_encoded_string (DECL_NAME (ns));

  print_indent (file, indent);
  fprintf (file,
           "<NamespaceAlias name=\"%s\">\n",
           name);
}


/* Print XML end tag for a namespace alias element.  */
static void
print_namespace_alias_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</NamespaceAlias>\n");
}


/* Print XML begin tag for a typedef element.  */
static void
print_typedef_begin_tag (file, indent, td)
  FILE* file;
  unsigned long indent;
  tree td;
{
  char* name = xml_get_encoded_string (DECL_NAME (td));

  print_indent (file, indent);
  fprintf (file,
           "<Typedef name=\"%s\">\n",
           name);
}


/* Print XML end tag for a typedef element.  */
static void
print_typedef_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Typedef>\n");  
}


/* Print XML begin tag for a class, struct, or union element.  */
static void
print_class_begin_tag (file, indent, rt)
  FILE* file;
  unsigned long indent;
  tree rt;
{
  char* name = xml_get_encoded_string (DECL_NAME (TYPE_NAME (rt)));
  
  print_indent (file, indent);
  if (TREE_CODE(rt) == RECORD_TYPE)
    {
    if (CLASSTYPE_DECLARED_CLASS (rt))
      {
      fprintf (file,
               "<Class name=\"%s\">\n",
               name);
      }
    else
      {
      fprintf (file,
               "<Struct name=\"%s\">\n",
               name);
      }
    }
  else
    {
      fprintf (file,
               "<Union name=\"%s\">\n",
               name);
    }
}


/* Print XML end tag for a class, struct, or union element.  */
static void
print_class_end_tag (file, indent, rt)
  FILE* file;
  unsigned long indent;
  tree rt;
{
  print_indent (file, indent);
  if (TREE_CODE(rt) == RECORD_TYPE)
    {
    if (CLASSTYPE_DECLARED_CLASS (rt))
      {
      fprintf (file,
               "</Class>\n");
      }
    else
      {
      fprintf (file,
               "</Struct>\n");
      }
    }
  else
    {
      fprintf (file,
               "</Union>\n");
    }
}


/* Print XML begin tag for a constructor element.  */
static void
print_constructor_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* access = "";

  if (TREE_PRIVATE (fd))        access = "private";
  else if (TREE_PROTECTED (fd)) access = "protected";
  else                          access = "public";  

  print_indent (file, indent);
  fprintf (file,
           "<Constructor access=\"%s\">\n",
           access);
}


/* Print XML end tag for a constructor element.  */
static void
print_constructor_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Constructor>\n");
}


/* Print XML begin tag for a destructor element.  */
static void
print_destructor_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* access = "";

  if (TREE_PRIVATE (fd))        access = "private";
  else if (TREE_PROTECTED (fd)) access = "protected";
  else                          access = "public";  

  print_indent (file, indent);
  fprintf (file,
           "<Destructor access=\"%s\">\n",
           access);
}


/* Print XML end tag for a destructor element.  */
static void
print_destructor_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Destructor>\n");
}


/* Print XML begin tag for a converter (type-conversion operator)
   element.  */
static void
print_converter_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* access = "";
  
  if (TREE_PRIVATE (fd))        access = "private";
  else if (TREE_PROTECTED (fd)) access = "protected";
  else                          access = "public";  
  
  print_indent (file, indent);
  fprintf (file,
           "<Converter access=\"%s\">\n",
           access);
}


/* Print XML end tag for a converter (type-conversion operator) element.  */
static void
print_converter_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Converter>\n");
}


/* Print XML begin tag for an operator function element.  */
static void
print_operator_function_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* name = xml_get_encoded_string (reverse_opname_lookup (DECL_NAME (fd)) );

  print_indent (file, indent);
  fprintf (file,
           "<OperatorFunction name=\"%s\">\n",
           name);
}


/* Print XML end tag for an operator function element.  */
static void
print_operator_function_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</OperatorFunction>\n");
}


/* Print XML begin tag for an operator method element.  */
static void
print_operator_method_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* name = xml_get_encoded_string (reverse_opname_lookup (DECL_NAME (fd)) );
  char* access = "";

  if (TREE_PRIVATE (fd))        access = "private";
  else if (TREE_PROTECTED (fd)) access = "protected";
  else                          access = "public";  

  print_indent (file, indent);
  fprintf (file,
           "<OperatorMethod name=\"%s\" access=\"%s\">\n",
           name, access);
}


/* Print XML end tag for an operator method element.  */
static void
print_operator_method_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</OperatorMethod>\n");
}


/* Print XML begin tag for a method element.  */
static void
print_method_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* name = xml_get_encoded_string (DECL_NAME (fd));
  char* access = "";
  int is_static;

  if (TREE_PRIVATE (fd))        access = "private";
  else if (TREE_PROTECTED (fd)) access = "protected";
  else                          access = "public";  

  if (DECL_NONSTATIC_MEMBER_FUNCTION_P (fd))
    is_static = 0;
  else
    is_static = 1;

  print_indent (file, indent);
  fprintf (file,
           "<Method name=\"%s\" access=\"%s\" static=\"%d\">\n",
           name, access, is_static);
}


/* Print XML end tag for a method element.  */
static void
print_method_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Method>\n");
}


/* Print XML begin tag for a function element.  */
static void
print_function_begin_tag (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  char* name = xml_get_encoded_string (DECL_NAME (fd));

  print_indent (file, indent);
  fprintf (file,
           "<Function name=\"%s\">\n",
           name);
}


/* Print XML end tag for a function element.  */
static void
print_function_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Function>\n");
}


/* Print XML begin tag for a function argument element.  */
static void
print_argument_begin_tag (file, indent, arg)
  FILE* file;
  unsigned long indent;
  tree arg;
{
  char* name = "";

  if(arg && DECL_NAME (arg))
    name = IDENTIFIER_POINTER (DECL_NAME (arg));
  
  print_indent (file, indent);
  fprintf (file,
           "<Argument name=\"%s\">\n",
           name);  
}


/* Print XML end tag for a function argument element.  */
static void
print_argument_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Argument>\n");
}


/* Print XML begin tag for a function return type element.  */
static void
print_returns_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<Returns>\n");
}


/* Print XML end tag for a function return type element.  */
static void
print_returns_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Returns>\n");
}


/* Print XML empty tag for a default argument element.  */
static void
print_default_argument_empty_tag (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  char* value;

  value = xml_get_encoded_string_from_string (
    expr_as_string (t, 0));
  
  print_indent (file, indent);
  fprintf (file,
           "<DefaultArgument value=\"%s\"/>\n",
           value);
}


/* Print XML empty tag for an ellipsis at the end of an argument list.  */
static void
print_ellipsis_empty_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<Ellipsis/>\n");
}


/* Print XML begin tag for a variable declaration element.  */
static void
print_variable_begin_tag (file, indent, vd)
  FILE* file;
  unsigned long indent;
  tree vd;
{
  char* name = IDENTIFIER_POINTER (DECL_NAME (vd));
  
  print_indent (file, indent);
  fprintf (file,
           "<Variable name=\"%s\">\n",
           name);
}


/* Print XML end tag for a variable declaration element.  */
static void
print_variable_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Variable>\n");
}


/* Print XML empty tag for a variable initializer element.  */
static void
print_initializer_empty_tag (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  char* value;
  
  if (!t || (t == error_mark_node)) return;
  
  value = xml_get_encoded_string_from_string (expr_as_string (t, 0));
  print_indent (file, indent);
  fprintf (file,
           "<Initializer value=\"%s\"/>\n",
           value);
}


/* Print XML begin tag for a field declaration element.  */
static void
print_field_begin_tag (file, indent, vd)
  FILE* file;
  unsigned long indent;
  tree vd;
{
  char* name = IDENTIFIER_POINTER (DECL_NAME (vd));
  
  print_indent (file, indent);
  fprintf (file,
           "<Field name=\"%s\">\n",
           name);
}


/* Print XML end tag for a field declaration element.  */
static void
print_field_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Field>\n");
}


/* Print XML begin tag for a const declaration element.  */
static void
print_enum_begin_tag (file, indent, vd)
  FILE* file;
  unsigned long indent;
  tree vd;
{
  char* name = IDENTIFIER_POINTER (DECL_NAME (vd));
  
  print_indent (file, indent);
  fprintf (file,
           "<Enum name=\"%s\">\n",
           name);
}


/* Print XML end tag for a const declaration element.  */
static void
print_enum_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Enum>\n");
}


/* Print XML begin tag for a type element.  */
static void
print_named_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);  
  fprintf (file,
           "<NamedType>\n");
}


/* Print XML end tag for a type element.  */
static void
print_named_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);  
  fprintf (file,
           "</NamedType>\n");
}


/* Print XML begin tag for a pointer type element.  */
static void
print_pointer_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<PointerType>\n");
}


/* Print XML end tag for a pointer type element.  */
static void
print_pointer_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file, "</PointerType>\n");
}


/* Print XML begin tag for a reference type element.  */
static void
print_reference_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<ReferenceType>\n");
}


/* Print XML end tag for a reference type element.  */
static void
print_reference_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file, "</ReferenceType>\n");
}


/* Print XML begin tag for a function type element.  */
static void
print_function_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<FunctionType>\n");
}


/* Print XML end tag for a function type element.  */
static void
print_function_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</FunctionType>\n");
}


/* Print XML begin tag for a method type element.  */
static void
print_method_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<MethodType>\n");
}


/* Print XML end tag for a method type element.  */
static void
print_method_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</MethodType>\n");
}


/* Print XML begin tag for a offset type element.  */
static void
print_offset_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<OffsetType>\n");
}


/* Print XML end tag for a offset type element.  */
static void
print_offset_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</OffsetType>\n");
}


/* Print XML begin tag for a array type element.  */
static void
print_array_type_begin_tag (file, indent, at)
  FILE* file;
  unsigned long indent;
  tree at;
{
  char* length = "";
  
  if (TYPE_DOMAIN (at))
    length = xml_get_encoded_string_from_string (
      expr_as_string (TYPE_MAX_VALUE (TYPE_DOMAIN (at)), 0));

  print_indent (file, indent);
  fprintf (file,
           "<ArrayType min=\"0\" max=\"%s\">\n",
           length);
}


/* Print XML end tag for a array type element.  */
static void
print_array_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</ArrayType>\n");
}


/* Print XML begin element tag for a base type element.  */
static void
print_base_type_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<BaseType>\n");
}


/* Print XML end element tag for a base type element.  */
static void
print_base_type_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</BaseType>\n");
}


/* Print XML begin element tag for a base class element.  */
static void
print_base_class_begin_tag (file, indent, binfo)
  FILE* file;
  unsigned long indent;
  tree binfo;
{
  char* access = "";

  if (TREE_VIA_PUBLIC (binfo))         access = "public";
  else if (TREE_VIA_PROTECTED (binfo)) access = "protected";
  else                                 access = "private";

  print_indent (file, indent);
  fprintf (file,
           "<BaseClass access=\"%s\">\n",
           access);
}


/* Print XML end element tag for a base class element.  */
static void
print_base_class_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</BaseClass>\n");
}


/* Print XML begin tag for a template instantiation element.  */
static void
print_instantiation_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<Instantiation>\n");
}


/* Print XML end tag for a template instantiation element.  */
static void
print_instantiation_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</Instantiation>\n");
}


/* Print XML begin tag for a template argument element.  */
static void
print_template_argument_begin_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<TemplateArgument>\n");
}


/* Print XML end tag for a template argument element.  */
static void
print_template_argument_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</TemplateArgument>\n");
}


/* Print XML empty tag indicating that the current declaration is
   external.  */
static void
print_external_empty_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<External/>\n");
}


/* Print XML empty tag indicating that the current type is incomplete.  */
static void
print_incomplete_type_empty_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "<IncompleteType/>\n");
}


/* Print XML empty tag describing the location of a declaration in the
   source.  */
static void
print_location_empty_tag (file, indent, d)
  FILE* file;
  unsigned long indent;
  tree d;
{  
  const char* source_file = DECL_SOURCE_FILE (d);
  unsigned int source_line = DECL_SOURCE_LINE (d);
  
  print_indent (file, indent);
  fprintf (file,
           "<Location file=\"%s\" line=\"%d\"/>\n",
           source_file, source_line);
}



/* Print XML empty tag describing the cv-qualifiers of a type.  */
static void
print_cv_qualifiers_empty_tag (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{  
  int is_const = CP_TYPE_CONST_P (t);
  int is_volatile = CP_TYPE_VOLATILE_P (t);
  int is_restrict = CP_TYPE_RESTRICT_P (t);  
  
  if (!is_const && !is_volatile && !is_restrict) return;
  
  print_indent (file, indent);
  fprintf (file,
           "<CV_Qualifiers const=\"%d\" volatile=\"%d\" restrict=\"%d\"/>\n",
           is_const, is_volatile, is_restrict);
}


/* Print XML begin tag for a name qualifier element.  */
static void
print_name_qualifier_begin_tag (file, indent, name)
  FILE* file;
  unsigned long indent;
  tree name;
{
  print_indent (file, indent);
  fprintf (file,
           "<NameQualifier name=\"%s\">\n",
           IDENTIFIER_POINTER (name));
}


/* Print XML end tag for a name qualifier element.  */
static void
print_name_qualifier_end_tag (file, indent)
  FILE* file;
  unsigned long indent;
{
  print_indent (file, indent);
  fprintf (file,
           "</NameQualifier>\n");
}


/* Print XML empty tag for a qualified name element.  */
static void
print_qualified_name_empty_tag (file, indent, name)
  FILE* file;
  unsigned long indent;
  tree name;
{
  print_indent (file, indent);
  fprintf (file,
           "<QualifiedName name=\"%s\"/>\n",
           IDENTIFIER_POINTER (name));
}


/* Print XML empty tag describing an unimplemented TREE_CODE that has been
   encountered.  */
static void
print_unimplemented_empty_tag (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  int tree_code = TREE_CODE (t);
  
  print_indent (file, indent);
  fprintf (file,
           "<Unimplemented tree_code=\"%d\" tree_code_name=\"%s\"/>\n",
           tree_code, tree_code_name [tree_code]);
}


/* Output the XML file's headers.  */
void
xml_output_headers (file)
  FILE* file;
{
  fprintf (file,
           "<?xml version=\"1.0\"?>\n");// encoding="ISO-8859-1"?>
}


/* Output a NAMESPACE_DECL.  Prints beginning and ending tags, and all
   the namespace's member declarations in between.  */
void
xml_output_namespace_decl (file, indent, ns)
  FILE* file;
  unsigned long indent;
  tree ns;
{
  if(ns == std_node) return;
    
  /* Only walk a real namespace.  */
  if (!DECL_NAMESPACE_ALIAS (ns))
    {
    tree cur_decl;

    print_namespace_begin_tag(file, indent, ns);

    for (cur_decl = cp_namespace_decls(ns); cur_decl;
         cur_decl = TREE_CHAIN (cur_decl))
      {
      switch (TREE_CODE (cur_decl))
        {
        case NAMESPACE_DECL:
          xml_output_namespace_decl(file, indent+XML_NESTED_INDENT, cur_decl);
          break;
        case TYPE_DECL:
          xml_output_type_decl(file, indent+XML_NESTED_INDENT, cur_decl);
          break;          
        case FUNCTION_DECL:
          xml_output_function_decl (file, indent+XML_NESTED_INDENT, cur_decl);
          break;
        case VAR_DECL:
          xml_output_var_decl (file, indent+XML_NESTED_INDENT, cur_decl);
          break;
        case TREE_LIST:
          xml_output_overload (file, indent+XML_NESTED_INDENT,
                               TREE_VALUE (cur_decl));
          break;
        case CONST_DECL:
          xml_output_const_decl (file, indent+XML_NESTED_INDENT, cur_decl);
          break;
        case TEMPLATE_DECL:
          xml_output_template_decl (file, indent+XML_NESTED_INDENT, cur_decl);
          break;
        case RESULT_DECL:
        case USING_DECL:
//        case THUNK_DECL:
          /* This is compiler-generated.  Just ignore it.  */
          break;
        default:
          print_unimplemented_empty_tag (file, indent+XML_NESTED_INDENT, cur_decl);
        }
      }
    
    print_namespace_end_tag(file, indent, ns);
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

    print_namespace_alias_begin_tag (file, indent, ns);
    xml_output_qualified_name (file, indent+XML_NESTED_INDENT, real_ns);
    print_namespace_alias_end_tag (file, indent);
    }
}


/* Dispatch output of a TYPE_DECL.  This is either a typedef, or a new
   type (class/struct/union) definition.  */
void
xml_output_type_decl (file, indent, td)
  FILE* file;
  unsigned long indent;
  tree td;
{
  /* Get the type as completely as possible.  */
  tree t = complete_type (TREE_TYPE (td));

  /* Don't process any internally generated declarations.  */
  if (MY_DECL_INTERNAL (td)) return;

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
    case ENUMERAL_TYPE:
    case FUNCTION_TYPE:
      /* A typedef to a predefined type.  */
      xml_output_typedef(file, indent, td);
      break;
    case RECORD_TYPE:
      if (TYPE_PTRMEMFUNC_P (t))
        {
        /* A typedef to a pointer to member.  */
        xml_output_typedef(file, indent, td);
        }
      else if (MY_TYPEDEF_TEST (td))
        {
        /* A typedef to an existing type.  */
        xml_output_typedef(file, indent, td);
        }
      else
        {
        /* A new type definition.  */
        xml_output_record_type(file, indent, t);
        }
      break;
    case UNION_TYPE:
      if (MY_TYPEDEF_TEST (td))
        {
        /* A typedef to an existing type.  */
        xml_output_typedef(file, indent, td);
        }
      else
        {
        /* A new type definition.  */
        xml_output_record_type(file, indent, t);
        }
      break;      
    default:
      print_unimplemented_empty_tag (file, indent, t);
    }
}


/* Output a RECORD_TYPE that is not a pointer-to-member-function.  Prints
   beginning and ending tags, and all class member declarations between.
   Also handles a UNION_TYPE.  */
void
xml_output_record_type (file, indent, rt)
  FILE* file;
  unsigned long indent;
  tree rt;
{
  tree field;
  tree func;

  print_class_begin_tag (file, indent, rt);
  print_location_empty_tag (file, indent+XML_NESTED_INDENT, TYPE_NAME (rt));

  if (!COMPLETE_TYPE_P (rt))
    {
    print_incomplete_type_empty_tag (file, indent+XML_NESTED_INDENT);
    }
  
  if (CLASSTYPE_TEMPLATE_INFO (rt))
    {
    xml_output_template_info (file, indent+XML_NESTED_INDENT,
                              CLASSTYPE_TEMPLATE_INFO (rt));
    }  
  
  /* Output all the non-method declarations in the class.  */
  for (field = TYPE_FIELDS (rt) ; field ; field = TREE_CHAIN (field))
    {
    switch(TREE_CODE(field))
      {
      case TYPE_DECL:
        /* A class or struct internally typedefs itself.  */
        if ((TREE_TYPE (field) == rt)
            && (DECL_NAME (field) == DECL_NAME (TYPE_NAME (rt))))
          xml_output_typedef (file, indent+XML_NESTED_INDENT, field);
        else
          xml_output_type_decl (file, indent+XML_NESTED_INDENT, field);
        break;          
      case FIELD_DECL:
        xml_output_field_decl (file, indent+XML_NESTED_INDENT, field);
        break;          
      case VAR_DECL:
        xml_output_var_decl (file, indent+XML_NESTED_INDENT, field);
        break;
      case CONST_DECL:
        xml_output_const_decl (file, indent+XML_NESTED_INDENT, field);
        break;
      case TEMPLATE_DECL:
        xml_output_template_decl (file, indent+XML_NESTED_INDENT, field);
        break;
      case RESULT_DECL:
      default:
        print_unimplemented_empty_tag (file, indent+XML_NESTED_INDENT, field);
      }
    }

  /* Output all the method declarations in the class.  */
  for (func = TYPE_METHODS (rt) ; func ; func = TREE_CHAIN (func))
    {
    switch (TREE_CODE (func))
      {
      case FUNCTION_DECL:
        xml_output_function_decl (file, indent+XML_NESTED_INDENT, func);
        break;
      case TEMPLATE_DECL:
        xml_output_template_decl (file, indent+XML_NESTED_INDENT, func);
        break;
      default:
        print_unimplemented_empty_tag (file, indent+XML_NESTED_INDENT, func);
      }
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
      xml_output_base_class (file, indent+XML_NESTED_INDENT, base_binfo);
      }
    }
  }
  
  print_class_end_tag (file, indent, rt);
}


/* Output for a FUNCTION_DECL.  Prints beginning and ending tags, and
   the argument list between them.  */
void
xml_output_function_decl (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  void (*end_tag_print)(FILE*, unsigned long) = NULL;
  tree arg;
  tree arg_type;

  /* Don't process any internally generated declarations.  */
  if (MY_DECL_INTERNAL (fd)) return;
  if (DECL_ARTIFICIAL (fd)) return;

  /* Print out the begin tag for this type of function.  */
  if (DECL_CONSTRUCTOR_P (fd))
    {
    /* A class constructor.  */
    print_constructor_begin_tag (file, indent, fd);
    end_tag_print = print_constructor_end_tag;
    }
  else if (DECL_DESTRUCTOR_P (fd))
    {
    /* A class destructor.  */
    print_destructor_begin_tag (file, indent, fd);
    end_tag_print = print_destructor_end_tag;
    }
  else if (DECL_OVERLOADED_OPERATOR_P (fd))
    {
    if (DECL_CONV_FN_P (fd))
      {
      /* A type-conversion operator in a class.  */
      print_converter_begin_tag (file, indent, fd);
      end_tag_print = print_converter_end_tag;
      }
    else
      {
      if (DECL_FUNCTION_MEMBER_P (fd))
        {
        /* An operator in a class.  */
        print_operator_method_begin_tag (file, indent, fd);
        end_tag_print = print_operator_method_end_tag;
        }
      else
        {
        /* An operator in a namespace.  */
        print_operator_function_begin_tag (file, indent, fd);
        end_tag_print = print_operator_function_end_tag;
        }
      }
    }
  else
    {
    if (DECL_FUNCTION_MEMBER_P (fd))
      {
      /* A member of a class.  */
      print_method_begin_tag (file, indent, fd);
      end_tag_print = print_method_end_tag;
      }
    else
      {
      /* A member of a namespace.  */
      print_function_begin_tag (file, indent, fd);
      end_tag_print = print_function_end_tag;
      }
    }

  /* Print the location information for this function.  */
  print_location_empty_tag (file, indent+XML_NESTED_INDENT, fd);

  if (DECL_TEMPLATE_INFO (fd))
    {
    xml_output_template_info (file, indent+XML_NESTED_INDENT,
                              DECL_TEMPLATE_INFO (fd));
    }

  if (DECL_REALLY_EXTERN (fd))
    {
    print_external_empty_tag (file, indent+XML_NESTED_INDENT);
    }

  if (!DECL_CONSTRUCTOR_P (fd) && !DECL_DESTRUCTOR_P (fd))
    {
    /* Print out the return type information for this function.  */
    xml_output_returns (file, indent+XML_NESTED_INDENT,
                        TREE_TYPE (TREE_TYPE (fd)));
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
    xml_output_argument (file, indent+XML_NESTED_INDENT, arg, arg_type);
    if(arg) arg = TREE_CHAIN (arg);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(!arg_type)
    {
    /* Function has variable number of arguments.  */
    print_ellipsis_empty_tag(file, indent+XML_NESTED_INDENT);
    }

  /* Print out the end tag for this type of function.  */
  if(end_tag_print)
    end_tag_print (file, indent);
}


/* Output for an OVERLOAD.  Output all the functions in the overload list.   */
void
xml_output_overload (file, indent, o)
  FILE* file;
  unsigned long indent;
  tree o;
{
  tree cur_overload = o;
  while (cur_overload)
    {
    xml_output_function_decl (file, indent, OVL_CURRENT (cur_overload));
    cur_overload = OVL_NEXT (cur_overload);
    }
}


/* Output for a VAR_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
void
xml_output_var_decl (file, indent, vd)
  FILE* file;
  unsigned long indent;
  tree vd;
{
  /* Don't process any internally generated declarations.  */
  if (MY_DECL_INTERNAL (vd)) return;

  print_variable_begin_tag (file, indent, vd);

  print_location_empty_tag (file, indent+XML_NESTED_INDENT, vd);

  if (DECL_EXTERNAL (vd))
    {
    print_external_empty_tag (file, indent+XML_NESTED_INDENT);
    }

  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (vd));

  print_initializer_empty_tag (file, indent+XML_NESTED_INDENT, DECL_INITIAL (vd));

  print_variable_end_tag (file, indent);
}


/* Output for a FIELD_DECL.  The name and type of the variable are output,
   as well as the initializer if it exists.  */
void
xml_output_field_decl (file, indent, fd)
  FILE* file;
  unsigned long indent;
  tree fd;
{
  /* Don't process any internally generated declarations.  */
  if (MY_DECL_INTERNAL (fd)) return;

  print_field_begin_tag (file, indent, fd);
  print_location_empty_tag (file, indent+XML_NESTED_INDENT, fd);
  // TODO: handle bit field case.
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (fd));
  print_field_end_tag (file, indent);
}


/* Output for a CONST_DECL.  The name and type of the constant are output,
   as well as the value.  */
void
xml_output_const_decl (file, indent, cd)
  FILE* file;
  unsigned long indent;
  tree cd;
{
  /* Don't process any internally generated declarations.  */
  if (MY_DECL_INTERNAL (cd)) return;

  print_enum_begin_tag (file, indent, cd);
  print_location_empty_tag (file, indent+XML_NESTED_INDENT, cd);
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (cd));
  print_initializer_empty_tag (file, indent+XML_NESTED_INDENT,
                          DECL_INITIAL (cd));
  print_enum_end_tag (file, indent);
}


/* Output for a TEMPLATE_DECL.  The set of specializations (including
   instantiations) is output.  */
void
xml_output_template_decl (file, indent, td)
  FILE* file;
  unsigned long indent;
  tree td;
{
  tree tl;
  
  /* Don't process any internally generated declarations.  */
  if (MY_DECL_INTERNAL (td)) return;

  for (tl = DECL_TEMPLATE_SPECIALIZATIONS (td);
       tl ; tl = TREE_CHAIN (tl))
    {
    tree ts = TREE_VALUE (tl);
    switch (TREE_CODE (ts))
      {
      case FUNCTION_DECL:
        xml_output_function_decl (file, indent, ts);
        break;
      case TEMPLATE_DECL:
        break;
      default:
        print_unimplemented_empty_tag (file, indent, ts);
      }
    }

  for (tl = DECL_TEMPLATE_INSTANTIATIONS (td);
       tl ; tl = TREE_CHAIN (tl))
    {
    tree ts = TYPE_NAME (TREE_VALUE (tl));
    switch (TREE_CODE (ts))
      {
      case TYPE_DECL:
        xml_output_type_decl (file, indent, ts);
        break;
      default:
        print_unimplemented_empty_tag (file, indent, ts);
      }
    }
}


/* Output for TEMPLATE_INFO.  The set of template parameters used
   is output.  */
void
xml_output_template_info (file, indent, ti)
  FILE* file;
  unsigned long indent;
  tree ti;
{
  tree arg_vec;
  int num_args;
  int i;

  print_instantiation_begin_tag (file, indent, ti);

  arg_vec = TI_ARGS (ti);
  num_args = TREE_VEC_LENGTH (arg_vec);
  for (i=0 ; i < num_args ; ++i)
    {
    tree arg = TREE_VEC_ELT (arg_vec, i);
    xml_output_template_argument (file, indent+XML_NESTED_INDENT, arg);
    }

  print_instantiation_end_tag (file, indent);  
}


/* Output for a typedef.  The name and associated type are output.  */
void
xml_output_typedef (file, indent, td)
  FILE* file;
  unsigned long indent;
  tree td;
{
  print_typedef_begin_tag (file, indent, td);
  print_location_empty_tag (file, indent+XML_NESTED_INDENT, td);
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (td));
  print_typedef_end_tag (file, indent);
}


/* Output for a *_TYPE.  This nests type definitions that contain other
   types (like pointers, references, functions, method, and arrays).  */
void
xml_output_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  switch (TREE_CODE (t))
    {
    case ARRAY_TYPE:
      xml_output_array_type (file, indent, t);
      break;
    case POINTER_TYPE:
      xml_output_pointer_type (file, indent, t);
      break;
    case REFERENCE_TYPE:
      xml_output_reference_type (file, indent, t);
      break;
    case FUNCTION_TYPE:
      xml_output_function_type (file, indent, t);
      break;
    case METHOD_TYPE:
      xml_output_method_type (file, indent, t);
      break;
    case RECORD_TYPE:
      if (TYPE_PTRMEMFUNC_P (t))
        xml_output_type (file, indent, TYPE_PTRMEMFUNC_FN_TYPE (t));
      else
        {
        xml_output_named_type (file, indent, t);
        }
      break;
    case OFFSET_TYPE:
      xml_output_offset_type (file, indent, t);
      break;
    case LANG_TYPE:
    case INTEGER_TYPE:
    case VOID_TYPE:
    case BOOLEAN_TYPE:
    case REAL_TYPE:
    case COMPLEX_TYPE:
    case ENUMERAL_TYPE:
    case UNION_TYPE:
      xml_output_named_type (file, indent, t);
      break;
    default:
      print_unimplemented_empty_tag (file, indent, t);
    }
}


/* Output for a normal named type.  */
void
xml_output_named_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{  
  print_named_type_begin_tag (file, indent, t);
  print_cv_qualifiers_empty_tag (file, indent+XML_NESTED_INDENT, t);
  xml_output_qualified_name (file, indent+XML_NESTED_INDENT,
                                   TYPE_MAIN_VARIANT (t));
  print_named_type_end_tag (file, indent);
}


/* Output for a FUNCTION_TYPE.  */
void
xml_output_function_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  tree arg_type;
  
  print_function_type_begin_tag (file, indent, t);  

  /* Print out the return type information for this function.  */
  xml_output_returns (file, indent+XML_NESTED_INDENT,
                      TREE_TYPE (t));
  
  /* Prepare to iterator through argument list.  */
  arg_type = TYPE_ARG_TYPES (t);

  /* Print out the argument list for this function.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (file, indent+XML_NESTED_INDENT, NULL, arg_type);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(arg_type != void_list_node)
    {
    /* Function has variable number of arguments.  */
    print_ellipsis_empty_tag(file, indent+XML_NESTED_INDENT);
    }

  print_function_type_end_tag (file, indent);
}


/* Output for a METHOD_TYPE.  */
void
xml_output_method_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  tree arg_type;
  
  print_method_type_begin_tag (file, indent);

  xml_output_base_type (file, indent+XML_NESTED_INDENT,
                        TYPE_METHOD_BASETYPE (t));                         

  /* Print out the return type information for this method.  */
  xml_output_returns (file, indent+XML_NESTED_INDENT,
                      TREE_TYPE (t));
  
  /* Prepare to iterator through argument list.  */
  arg_type = TYPE_ARG_TYPES (t);

  /* Skip "this" argument.  */
  arg_type = TREE_CHAIN (arg_type);

  /* Print out the argument list for this method.  */
  while (arg_type && (arg_type != void_list_node))
    {
    xml_output_argument (file, indent+XML_NESTED_INDENT, NULL, arg_type);
    arg_type = TREE_CHAIN (arg_type);
    }

  if(arg_type != void_list_node)
    {
    /* Method has variable number of arguments.  */
    print_ellipsis_empty_tag(file, indent+XML_NESTED_INDENT);
    }

  print_cv_qualifiers_empty_tag (file, indent+XML_NESTED_INDENT, t);

  print_method_type_end_tag (file, indent);
}


/* Output for a POINTER_TYPE.  */
void
xml_output_pointer_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  print_pointer_type_begin_tag (file, indent, t);
  print_cv_qualifiers_empty_tag (file, indent+XML_NESTED_INDENT, t);
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (t));
  print_pointer_type_end_tag (file, indent);
}


/* Output for a REFERENCE_TYPE.  */
void
xml_output_reference_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  print_reference_type_begin_tag (file, indent, t);
  print_cv_qualifiers_empty_tag (file, indent+XML_NESTED_INDENT, t);
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (t));
  print_reference_type_end_tag (file, indent);
}


/* Output for an OFFSET_TYPE.  */
void
xml_output_offset_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  print_offset_type_begin_tag (file, indent);
  xml_output_base_type (file, indent+XML_NESTED_INDENT,
                        TYPE_OFFSET_BASETYPE (t));
  print_cv_qualifiers_empty_tag (file, indent+XML_NESTED_INDENT, t);
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (t));
  print_offset_type_end_tag (file, indent);
}


/* Output for an ARRAY_TYPE.  */
void
xml_output_array_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  print_array_type_begin_tag (file, indent, t);
  print_cv_qualifiers_empty_tag (file, indent+XML_NESTED_INDENT, t);
  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_TYPE (t));
  print_array_type_end_tag (file, indent);
}


/* Output a base class for a RECORD_TYPE or UNION_TYPE.  */
void
xml_output_base_class (file, indent, binfo)
  FILE* file;
  unsigned long indent;
  tree binfo;
{
  print_base_class_begin_tag (file, indent, binfo);
  xml_output_qualified_name (file, indent+XML_NESTED_INDENT,
                                   BINFO_TYPE (binfo));
  print_base_class_end_tag (file, indent);
}


/* Output a base type for a METHOD_TYPE or OFFSET_TYPE.  */
void
xml_output_base_type (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  print_base_type_begin_tag (file, indent);
  xml_output_qualified_name (file, indent+XML_NESTED_INDENT, t);
  print_base_type_end_tag (file, indent);
}


/* Output for a PARM_DECL / TREE_LIST corresponding to a function argument.  */
void
xml_output_argument (file, indent, pd, tl)
  FILE* file;
  unsigned long indent;
  tree pd;
  tree tl;
{
  print_argument_begin_tag (file, indent, pd);

  xml_output_type (file, indent+XML_NESTED_INDENT, TREE_VALUE (tl));
  
  if (TREE_PURPOSE (tl))
    print_default_argument_empty_tag (file, indent+XML_NESTED_INDENT,
                                 TREE_PURPOSE (tl));
  // Output something for DECL_ARG_TYPE (pd)  ??
  print_argument_end_tag (file, indent);
}


/* Output the return type of a function.  */
void
xml_output_returns (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  print_returns_begin_tag (file, indent);
  xml_output_type (file, indent+XML_NESTED_INDENT, t);
  print_returns_end_tag (file, indent);
}


/* Output the given template argument.  */
void
xml_output_template_argument (file, indent, arg)
  FILE* file;
  unsigned long indent;
  tree arg;
{
  print_template_argument_begin_tag (file, indent);
  print_unimplemented_empty_tag (file, indent+XML_NESTED_INDENT, arg);
  print_template_argument_end_tag (file, indent);
}


/* Given a list of qualifiers, walk it recursively, and output it in
   nested form.  */
static void
output_qualifier_list (file, indent, qualifiers)
  FILE* file;
  unsigned long indent;
  tree qualifiers;
{
  tree next = TREE_CHAIN (qualifiers);
  tree decl = TREE_VALUE (qualifiers);
  
  if (next)
    {
    print_name_qualifier_begin_tag (file, indent, DECL_NAME (decl));
    output_qualifier_list (file, indent+XML_NESTED_INDENT, next);
    print_name_qualifier_end_tag (file, indent);
    }
  else
    {
    print_qualified_name_empty_tag (file, indent, DECL_NAME (decl));
    }
}


/* Given any _DECL, output the tags of the fully qualified form of its
   name.  */
void
xml_output_qualified_name (file, indent, t)
  FILE* file;
  unsigned long indent;
  tree t;
{
  tree context;
  tree qualifiers = NULL_TREE;
  
  /* Build a list of all the qualifying contexts out to the global
     namespace.  */
  context = t;
  while (context && (context != global_namespace))
    {
    if (TYPE_P (context)) context = TYPE_NAME (context);

    qualifiers = tree_cons (NULL_TREE, context, qualifiers);

    context = CP_DECL_CONTEXT (context);
    }
  
  /* Output the qualifiers in the list.  */
  if (qualifiers)
    output_qualifier_list (file, indent, qualifiers);
}


/* Convert the identifier IN_ID to an XML encoded form by passing its string
   to xml_get_encoded_string_from_string().  */
char*
xml_get_encoded_string (in_id)
  tree in_id;
{
  if(in_id)
    return xml_get_encoded_string_from_string (IDENTIFIER_POINTER (in_id));
  else
    return "";
}

/*
  #define XML_AMPERSAND     "&#x0026;"
  #define XML_LESS_THAN     "&#x003C;"
  #define XML_GREATER_THAN  "&#x003E;"
*/

#define XML_AMPERSAND     "&amp;"
#define XML_LESS_THAN     "&lt;"
#define XML_GREATER_THAN  "&gt;"

/* Convert the name IN_STR to an XML encoded form.
   This replaces '&', '<', and '>'
   with their corresponding unicode character references.  */
char*
xml_get_encoded_string_from_string (in_str)
  const char* in_str;
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
      }
    }

  newStr = (char*) malloc (strlen (in_str) + length_increase + 1);
  outCh = newStr;

  /* Create encoded form of string.  */
  for(inCh = in_str ; *inCh != '\0' ; ++inCh)
    {
    switch (*inCh)
      {
      case '&': strcpy (outCh, XML_AMPERSAND); outCh += strlen(XML_AMPERSAND); break;
      case '<': strcpy (outCh, XML_LESS_THAN);  outCh += strlen(XML_LESS_THAN); break;
      case '>': strcpy (outCh, XML_GREATER_THAN);  outCh += strlen(XML_GREATER_THAN); break;
      default: *outCh++ = *inCh;
      }
    }  

  *outCh = '\0';

  /* Ask for "identifier" of this string and free our own copy of the
     memory.  */
  id = get_identifier(newStr);
  free(newStr);
  return IDENTIFIER_POINTER (id);
}

#undef XML_AMPERSAND
#undef XML_LESS_THAN
#undef XML_GREATER_THAN

/* Print N spaces to FILE.  Used for indentation of XML output.  */
void
print_indent (file, n)
  FILE* file;
  unsigned long n;
{
  unsigned long left = n;
  while(left >= 10)
    {
    fprintf(file, "          ");
    left -= 10;
    }
  while(left > 0)
    {
    fprintf(file, " ");
    left -= 1;
    }
}


/* Lookup the real name of an operator whose ansi_opname or ansi_assopname
   is NAME.  */
tree
reverse_opname_lookup (name)
  tree name;
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



#if 0
static char* xml_concat_3_strings          PARAMS ((char*, char*, char*));
static tree  xml_get_qualified_type_name   PARAMS ((tree));

/* Concatenate input strings into new memory.  Caller must free memory.  */
char*
xml_concat_3_strings (s1, s2, s3)
  char* s1;
  char* s2;
  char* s3;
{
  char* ns = (char*) malloc (strlen (s1) +  strlen (s2) + strlen (s3) + 1);
  strcpy (ns, s1);
  strcat (ns, s2);
  strcat (ns, s3);
  return ns;
}

/* Given a type, return an identifier node with the fully qualified type
   name. */
tree
xml_get_qualified_type_name (t)
  tree t;
{
  char* temp_name;
  char* name;
  tree context = t;
  tree result;
  
  if (!TYPE_NAME (t)) return get_identifier("{anonymous type}");

  name = strdup (IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (t))));

  /* Walk down nested classes/structs/unions.  */
  context = CP_DECL_CONTEXT (TYPE_NAME (t));
  while (TREE_CODE (context) != NAMESPACE_DECL)
    {
    temp_name = xml_concat_3_strings (
      IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (context))), "::", name);
    free (name);
    name = temp_name;
    context = CP_DECL_CONTEXT (TYPE_NAME (context));
    }
  
  /* Walk down nested namespaces.  */
  while (context != global_namespace)
    {
    temp_name = xml_concat_3_strings (
      IDENTIFIER_POINTER (DECL_NAME (context)), "::", name);
    free (name);
    name = temp_name;
    context = CP_DECL_CONTEXT (context);
    }
  
  result = get_identifier (name);
  free (name);
  return result;
}
#endif
