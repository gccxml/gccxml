/*=========================================================================

  Program:   GCC-XML
  Module:    gxDocumentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "gxDocumentation.h"
#include "gxConfigure.h"

//----------------------------------------------------------------------------
struct gxDocumentationEntry
{
  const char* name;
  const char* brief;
  const char* full;
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationName[] =
{
  {"gccxml",
   "- Create an XML representation of C++ declarations.", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationUsage[] =
{
  {0,
   "gccxml [options] <input-file> -fxml=<output-file>", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationDescription[] =
{
  {0,
   "GCC-XML parses a C++ source file as it is seen by the compiler when it "
   "is built.  An easy-to-parse XML representation of the class, function, "
   "and namespace declarations is dumped to a specified file.  Full C "
   "preprocessing transforms the file into a C++ translation unit as seen "
   "by the compiler.  This means that GCC-XML should make use of the same "
   "standard library and other header files as the compiler.  GCC-XML can "
   "be configured to simulate any of several popular compilers. ", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationCompilers[] =
{
  {0, "GCC-XML can simulate any of the following compilers:", 0},
  {"GCC", "Versions 3.2.x, 3.1.x, 3.0.x, and 2.95.x", 0},
  {"SGI MIPSpro", "Version 7.3x", 0},
  {"Visual C++", "Versions 7 and 6 (sp5)", 0},
  {"Intel C++", "Version 5.x (plugin to Visual Studio 6)", 0},
  {0,
   "Advanced users can simulate other compilers by manually configuring "
   "the GCCXML_FLAGS setting.  Contact the mailing list for help.", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationOptions[] =
{
  {0,
   "The following options are available for running GCC-XML:", 0},
  {"--copyright", "Print the GCC-XML copyright and exit.", 0},
  {"--debug", "Print extra debugging information.",
   "This option causes GCC-XML to print the executable name and "
   "command-line arguments used to execute the patched GCC C++ parser.  "
   "This is useful when attempting to simulate an unsupported compiler."},
  {"-fxml=<output-file>", "Specify the XML output file.",
   "This option is passed directly on to the patched GCC C++ parser.  It "
   "enables the XML dump and specifies the output file name."},
  {"-fxml-start=<xxx>[,...]", "Specify a list of starting declarations.",
   "This option is passed directly on to the patched GCC C++ parser.  It "
   "is meaningful only if -fxml= is also specified.  This specifies a "
   "comma-separated list of named starting declarations.  GCC-XML will "
   "dump only the subset of the declarations in the translation unit that "
   "is reachable through a sequence of source references from one of the "
   "specified starting declarations."},
  {"--gccxml-compiler <xxx>", "Set GCCXML_COMPILER to \"xxx\".", 0},
  {"--gccxml-cxxflags <xxx>", "Set GCCXML_CXXFLAGS to \"xxx\".", 0},
  {"--gccxml-executable <xxx>", "Set GCCXML_EXECUTABLE to \"xxx\".", 0},
  {"--gccxml-config <xxx>", "Set GCCXML_CONFIG to \"xxx\".", 0},
  {"--gccxml-root <xxx>", "Set GCCXML_ROOT to \"xxx\".", 0},
  {"--help", "Print full help and exit.",
   "Full help displays most of the documentation provided by the UNIX "
   "man page.  It is provided for use on non-UNIX platforms, but is "
   "also convenient if the man page is not installed."},
  {"--help-html", "Print full help in HTML format.",
   "This option is used by GCC-XML authors to help produce web pages."},
  {"--man", "Print a UNIX man page and exit.",
   "This option is used by GCC-XML authors to generate the UNIX man page."},
  {"--print", "Print configuration settings and exit.",
   "GCC-XML has many configuration options to help it simulate another "
   "compiler.  Using this option will cause GCC-XML to configure itself "
   "as if it were going to parse the C++ source, but stop and print the "
   "configuration found.  This is useful for checking the configuration."},
  {"--version", "Show program name/version banner and exit.", 0},
  {0,
   "Other flags, such as -I and -D, are passed on to the patched GCC C++ "
   "parser executable.", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationSettings[] =
{
  {0,
   "GCC-XML is designed to simulate a compiler's parser while reading "
   "C++ source code.  Some configuration settings are needed to determine "
   "how to simulate a particular compiler of the user's choice.  The "
   "following settings can be used to configure GCC-XML:", 0},
  {"GCCXML_COMPILER", "The C++ compiler to be simulated.",
   "GCC-XML will attempt to automatically determine how to simulate "
   "the compiler specified by this setting.  The compiler is specified "
   "by its executable name (such as \"g++\").  For Visual Studio, the "
   "compiler is specified by \"msvc6\" or \"msvc7\" (if \"cl\" is given, "
   "GCC-XML usually chooses msvc6 or msvc7 correctly)."},
  {"GCCXML_CXXFLAGS", "The flags for the C++ compiler to be simulated.",
   "The behavior of most compilers can be adjusted by specifying flags on "
   "the command line.  When GCC-XML attempts to automatically determine "
   "how to simulate a compiler, these flags are taken into consideration."},
  {"GCCXML_CONFIG", "The configuration file for common settings.",
   "When non-default settings are often used, it is convenient to write "
   "a single file containing them.  When such a file is specified, it will "
   "be read to configure any settings that are not yet known.  Each "
   "line of the file consists of one assignment of the form KEY=\"VALUE\" "
   "(for example, GCCXML_COMPILER=\"g++\")."},
  {"GCCXML_EXECUTABLE", "Specify the patched GCC C++ parser executable.",
   "The GCC-XML program as seen by the user is actually a front-end that "
   "determines the flags needed to configure the patched GCC C++ parser to "
   "simulate another compiler.  This setting specifies the real executable "
   "to run once the flags have been determined.  Users should rarely need "
   "to change this value from its default."},
  {"GCCXML_ROOT", "The GCC-XML support library directory.",
   "Since GCC-XML is only one C++ parser, it cannot exactly duplicate the "
   "functionality of every compiler it tries to simulate.  Some compilers "
   "provide standard headers with code that GCC-XML cannot directly "
   "handle.  To work around this limitation, a support library is "
   "provided for each compiler.  This consists of a set of header files "
   "that are used in place of the compiler's system headers.  These files "
   "contain slight tweaks and then include the corresponding real "
   "header.  The root of the directory tree containing these support "
   "library headers is specified by this setting.  Users should rarely "
   "need to change this value from its default."},
  {"GCCXML_FLAGS", "Flags used to simulate the other compiler.",
   "When GCC-XML runs the patched GCC C++ parser, these flags are passed "
   "to the program to tell it how to simulate a particular compiler.  This "
   "setting is usually detected automatically from the other settings, but "
   "it can be specified directly by advanced users.  Most users should "
   "not attempt to change this value from the automatic configuration."},
  {"GCCXML_USER_FLAGS", "Additional user flags for compiler simulation.",
   "When GCC-XML runs the patched GCC C++ parser, these flags are passed "
   "in addition to those specified by GCCXML_FLAGS.  This allows advanced "
   "users to tweak the compiler simulation while still using the automatic "
   "configuration of GCCXML_FLAGS.  Users should rarely need to change this "
   "value from its default."},
  {0,
   "There are several means by which these settings are configured.  They "
   "are listed here in order of precedence (highest first):", 0},
  {"Command-line Options",
   "Settings can be specified by their corresponding options.",
   "When a setting's corresponding command-line option is provided, "
   "it is used in favor over any other means of configuration.  If "
   "GCCXML_CONFIG is set on the command-line, settings are read from the "
   "file with precedence just slightly lower than other command-line "
   "options."},
  {"Environment Variables",
   "Settings are configured by name in the environment.",
   "Each setting not already known is read from an environment variable "
   "with its name.  If GCCXML_CONFIG is set by the environment, settings "
   "are read from the file with precedence just slightly lower than other "
   "environment variables."},
  {"Configuration Files", "A search for GCCXML_CONFIG is performed.",
   "If GCCXML_CONFIG has not yet been set, an attempt is made to find a "
   "configuration file automatically.  First, if the file "
   "$HOME/.gccxml/config exists, it will be used.  Second, if GCC-XML "
   "is being executed from its build directory, a config file from "
   "that directory will be used.  Finally, if a config file is found in "
   "the installation's support library directory, it will be used.  "
   "Once found, any unknown settings are read from the configuration file."},
  {"Guessing", "Guesses are made based on other settings.",
   "Once GCCXML_COMPILER has been set, it is used to automatically find "
   "the setting for GCCXML_FLAGS.  If it is not set, the \"CXX\" "
   "environment variable is checked as a last-resort to find the compiler "
   "setting and determine GCCXML_FLAGS."},
  {0,
   "Most users should not have to adjust the defaults for these settings.  "
   "There is a default GCCXML_CONFIG file provided in the support library "
   "directory after installation.  It configures GCC-XML to simulate the "
   "compiler that was used to build it.", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
const gxDocumentationEntry gxDocumentationCopyright[] =
{
  {0,
   "Copyright (c) 2002 Kitware, Inc., Insight Consortium.\n"
   "All rights reserved.\n", 0},
  {0,
   "Redistribution and use in source and binary forms, with or without "
   "modification, are permitted provided that the following conditions are "
   "met:\n", 0},
  {" * ",
   "Redistributions of source code must retain the above copyright notice, "
   "this list of conditions and the following disclaimer.\n", 0},
  {" * ",
   "Redistributions in binary form must reproduce the above copyright "
   "notice, this list of conditions and the following disclaimer in the "
   "documentation and/or other materials provided with the distribution.\n",
   0},
  {" * ",
   "The names of Kitware, Inc., the Insight Consortium, or the names of "
   "any consortium members, or of any contributors, may not be used to "
   "endorse or promote products derived from this software without "
   "specific prior written permission.\n", 0},
  {" * ",
   "Modified source versions must be plainly marked as such, and must "
   "not be misrepresented as being the original software.\n", 0},
  {0,
   "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "
   "``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
   "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
   "A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR "
   "CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
   "EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
   "PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
   "PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
   "LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
   "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
   "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n", 0},
  {0, 0, 0}
};

//----------------------------------------------------------------------------
void gxDocumentationPrintManSection(std::ostream& os,
                                    const gxDocumentationEntry* section)
{
  for(const gxDocumentationEntry* op = section; op->brief; ++op)
    {
    if(op->name)
      {
      os << ".TP\n"
         << ".B " << op->name << "\n"
         << op->brief << "\n";
      if(op->full) { os << op->full << "\n"; }
      }
    else
      {
      os << ".PP\n"
         << op->brief << "\n";
      }
    }  
}

//----------------------------------------------------------------------------
void gxDocumentationPrintHelpSection(std::ostream& os,
                                     const gxDocumentationEntry* section)
{
  for(const gxDocumentationEntry* op = section; op->brief; ++op)
    {
    if(op->name)
      {
      os << "  " << op->name << "\n"
         << "       ";
      gxDocumentation::PrintColumn(os, 70, "       ", op->brief);
      if(op->full)
        {
        os << "\n"
           << "       ";
        gxDocumentation::PrintColumn(os, 70, "       ", op->full);
        }
      os << "\n";
      }
    else
      {
      gxDocumentation::PrintColumn(os, 77, "", op->brief);
      os << "\n";
      }
    os << "\n";
    }  
}

//----------------------------------------------------------------------------
void gxDocumentationPrintHTMLEscapes(std::ostream& os, const char* text)
{
  static gxDocumentationEntry escapes[] =
  {
    {"<", "&lt;", 0},
    {">", "&gt;", 0},
    {"&", "&amp;", 0},
    {0,0,0}
  };
  for(const char* p = text; *p; ++p)
    {
    bool found = false;
    for(const gxDocumentationEntry* op = escapes; !found && op->name; ++op)
      {
      if(op->name[0] == *p)
        {
        os << op->brief;
        found = true;
        }
      }
    if(!found)
      {
      os << *p;
      }
    }
}

//----------------------------------------------------------------------------
void gxDocumentationPrintHelpHTMLSection(std::ostream& os,
                                         const gxDocumentationEntry* section)
{
  for(const gxDocumentationEntry* op = section; op->brief;)
    {
    if(op->name)
      {
      os << "<ul>\n";
      for(;op->name;++op)
        {
        os << "  <li>\n";
        os << "    <b><code>";
        gxDocumentationPrintHTMLEscapes(os, op->name);
        os << "</code></b>: ";
        gxDocumentationPrintHTMLEscapes(os, op->brief);
        if(op->full)
          {
          os << "  ";
          gxDocumentationPrintHTMLEscapes(os, op->full);
          }
        os << "\n";
        os << "  </li>\n";
        }
      os << "</ul>\n";
      }
    else
      {
      gxDocumentationPrintHTMLEscapes(os, op->brief);
      os << "\n";
      ++op;
      }
    }
}

//----------------------------------------------------------------------------
void gxDocumentationPrintUsageSection(std::ostream& os,
                                      const gxDocumentationEntry* section)
{
  std::ios::fmtflags flags = os.flags();
  os.setf(flags | std::ios::left);
  for(const gxDocumentationEntry* op = section; op->brief; ++op)
    {
    if(op->name)
      {
      os << "  ";
      os.width(25);
      os << op->name << "= " << op->brief << "\n";
      }
    else
      {
      os << "\n";
      gxDocumentation::PrintColumn(os, 74, "", op->brief);
      os << "\n";
      }
    }  
  os.setf(flags);
}

//----------------------------------------------------------------------------
void gxDocumentation::PrintUsage(std::ostream& os)
{
  os << "Usage:\n";
  gxDocumentationPrintUsageSection(os, gxDocumentationUsage);
  gxDocumentationPrintUsageSection(os, gxDocumentationOptions);
}

//----------------------------------------------------------------------------
void gxDocumentation::PrintHelp(std::ostream& os)
{
  os << "Usage:\n";
  os << "\n";
  gxDocumentationPrintHelpSection(os, gxDocumentationUsage);
  gxDocumentationPrintHelpSection(os, gxDocumentationDescription);
  os << "----------------------------------------------------------\n";
  gxDocumentationPrintHelpSection(os, gxDocumentationOptions);
  os << "----------------------------------------------------------\n";
  gxDocumentationPrintHelpSection(os, gxDocumentationSettings);
  os << "----------------------------------------------------------\n";
  gxDocumentationPrintHelpSection(os, gxDocumentationCompilers);
}

//----------------------------------------------------------------------------
void gxDocumentation::PrintHelpHTML(std::ostream& os)
{
  os << "<html>\n"
     << "<body>\n";
  os << "<h2>How to Run GCC-XML</h2>\n";
  os << "<blockquote><code>\n";
  gxDocumentationPrintHelpHTMLSection(os, gxDocumentationUsage);
  os << "</code></blockquote>\n";
  gxDocumentationPrintHelpHTMLSection(os, gxDocumentationDescription);
  os << "<h2>Command-line Options</h2>\n";
  gxDocumentationPrintHelpHTMLSection(os, gxDocumentationOptions);
  os << "<h2>Configuration Settings</h2>\n";
  gxDocumentationPrintHelpHTMLSection(os, gxDocumentationSettings);
  os << "<h2>Supported Compilers</h2>\n";
  gxDocumentationPrintHelpHTMLSection(os, gxDocumentationCompilers);
  os << "</body>\n"
     << "</html>\n";
}

//----------------------------------------------------------------------------
void gxDocumentation::PrintManPage(std::ostream& os)
{
  os << ".TH GCC-XML 1 \""
     << gxSystemTools::GetCurrentDateTime("%B %d, %Y").c_str()
     << "\" \"GCC-XML " GCCXML_VERSION_STRING "\"\n";
  os << ".SH NAME\n";
  gxDocumentationPrintManSection(os, gxDocumentationName);
  os << ".SH SYNOPSIS\n";
  gxDocumentationPrintManSection(os, gxDocumentationUsage);
  os << ".SH DESCRIPTION\n";
  gxDocumentationPrintManSection(os, gxDocumentationDescription);
  os << ".SH OPTIONS\n";
  gxDocumentationPrintManSection(os, gxDocumentationOptions);
  os << ".SH SETTINGS\n";
  gxDocumentationPrintManSection(os, gxDocumentationSettings);
  os << ".SH COMPILERS\n";
  gxDocumentationPrintManSection(os, gxDocumentationCompilers);
  os << ".SH COPYRIGHT\n";
  gxDocumentationPrintManSection(os, gxDocumentationCopyright);
  os << ".SH MAILING LIST\n";
  os << "For help and discussion about using gccxml, a mailing list is\n"
     << "provided at\n"
     << ".B gccxml@www.gccxml.org.\n"
     << "Please first read the full documentation at\n"
     << ".B http://www.gccxml.org\n"
     << "before posting questions to the list.\n";
  os << ".SH AUTHOR\n"
     << "This manual page was generated by \"gccxml --man\".\n";
}

//----------------------------------------------------------------------------
void gxDocumentation::PrintCopyright(std::ostream& os)
{
  os << "GCC-XML version " GCCXML_VERSION_STRING "\n";
  for(const gxDocumentationEntry* op = gxDocumentationCopyright;
      op->brief; ++op)
    {
    if(op->name)
      {
      os << " * ";
      gxDocumentation::PrintColumn(os, 74, "   ", op->brief);
      }
    else
      {
      gxDocumentation::PrintColumn(os, 77, "", op->brief);
      }
    os << "\n";
    }
}

//----------------------------------------------------------------------------
void gxDocumentation::PrintColumn(std::ostream& os, int width,
                                  const char* indent, const char* text)
{
  // Print text arranged in a column of fixed witdh indented by the
  // "indent" text.
  const char* l = text;
  int column = 0;
  bool newSentence = false;
  bool first = true;
  while(*l)
    {
    // Parse the next word.
    const char* r = l;
    while(*r && (*r != '\n') && (*r != ' ')) { ++r; }
    
    // Does it fit on this line?
    if(r-l < (width-column-(newSentence?1:0)))
      {
      // Word fits on this line.
      if(r > l)
        {
        if(column)
          {
          // Not first word on line.  Separate from the previous word
          // by a space, or two if this is a new sentence.
          if(newSentence)
            {
            os << "  ";
            column += 2;
            }
          else
            {
            os << " ";
            column += 1;
            }
          }
        else
          {
          // First word on line.  Print indentation unless this is the
          // first line.
          os << (first?"":indent);
          }
        
        // Print the word.
        os.write(l, static_cast<long>(r-l));
        newSentence = (*(r-1) == '.');
        }
      
      if(*r == '\n')
        {
        // Text provided a newline.  Start a new line.
        os << "\n";
        ++r;
        column = 0;
        first = false;
        }
      else
        {
        // No provided newline.  Continue this line.
        column += static_cast<long>(r-l);
        }
      }
    else
      {
      // Word does not fit on this line.  Start a new line.
      os << "\n";
      first = false;
      if(r > l)
        {
        os << indent;
        os.write(l, static_cast<long>(r-l));
        column = static_cast<long>(r-l);
        newSentence = (*(r-1) == '.');
        }
      }
    
    // Move to beginning of next word.  Skip over whitespace.
    l = r;
    while(*l && (*l == ' ')) { ++l; }    
    }
}
