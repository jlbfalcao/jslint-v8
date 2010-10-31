/*
    Copyright (c) 2010 Jorge Falcão basead in js-beautify code 
 */

#include <v8.h>

#include <string.h>

#include "jslint.h"
#include "v8.h"

using namespace v8;

Handle<String> readFile(const char* name) {
    FILE* file = fopen(name, "rb");
    
    if (file == NULL) {
        return Handle<String>();
    }
    
    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);
  
    char* buf = new char[len + 1];
    buf[len] = '\0';
    
    fread(buf, 1, len, file);

    fclose(file);
    
    Handle<String> result = String::New(buf);
    
    delete[] buf;
    
    return result;
}

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> Print(const v8::Arguments& args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope;
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    v8::String::Utf8Value str(args[i]);
    const char* cstr = ToCString(str);
    printf("%s", cstr);
  }
  printf("\n");
  fflush(stdout);
  return v8::Undefined();
}


static void usage(char* progname)
{
    
    printf("Usage:  %s [options] source-file\n", progname);
    printf("See: http://www.JSLint.com/lint.html\n");
    printf("default selected: --bitwise --eqeqeq --immed --newcap --nomen --onevar --regexp --undef --white\n");
    printf("[options]\n");
    printf(" --adsafe      : ADsafe\n");
    printf(" --bitwise     : Disallow bitwise operator.\n"); 
    printf(" --browser     : Assume a browser.\n");
    printf(" --cap         : Tolerate HTML case.\n"); 
    printf(" --css         : Tolerate CSS workarounds.\n");
    printf(" --debug       : Tolerate debugger statements.\n");
    printf(" --devel       : Assume console, alert, ...\n");
    printf(" --eqeqeq      : Disallow == and !=\n");
    printf(" --es5         : Tolerate ES5 syntax\n");
    printf(" --evil        : Tolerate eval\n");
    printf(" --forin       : Tolerate unfiltered for in.\n"); 
    printf(" --fragment    : Tolerate HTML fragments.\n"); 
    printf(" --immed       : Require parens around immediate invocations.\n");
    printf(" --indent      : Strict white space indentation.\n");
    printf(" --laxbreak    : Tolerate sloppy line breaking.\n");
    printf(" --maxerr      : Maximum number of errors.\n");
    printf(" --maxlen      : Maximum line length.\n");
    printf(" --nomen       : Disallow dangling _ in identifiers.\n");
    printf(" --newcap      : Require Initial Caps for constructors on 	 Tolerate HTML event handlers.\n"); 
    printf(" --onevar      : Allow one var statement per function.\n");
    printf(" --passfail    : Stop on first error.\n");
    printf(" --plusplus    : Disallow ++ and --.\n");
    printf(" --predef      : Predefined ( , separated).\n");
    printf(" --regexp      : Disallow insecure . and [^...]. in /RegExp/.\n");
    printf(" --rhino       : Assume Rhino.\n");
    printf(" --safe        : Safe Subset .\n");
    printf(" --strict      : Require \"use strict\";.\n"); 
    printf(" --sub         : Tolerate inefficient subscripting.\n");
    printf(" --undef       : Disallow undefined variables.\n");
    printf(" --white       : Strict white space.\n");
    printf(" --widget      : Assume a Yahoo Widget.\n");
    printf(" --windows     : AssumeWindows.\n");
}

int main(int argc, char* argv[])
{
    Handle<String> source;
    HandleScope handle_scope;

    Handle<ObjectTemplate> global = ObjectTemplate::New();
    Handle<ObjectTemplate> options = ObjectTemplate::New();
    
    bool overwrite = false;
    const char* output = 0;
    bool some_parameter = false;
    
    for (int argpos = 1; argpos < argc; ++argpos) {
        if (argv[argpos][0] != '-') {
            source = readFile(argv[argpos]);
            output = argv[argpos];

        } else if (strcmp(argv[argpos], "--help") == 0 ||
                   strcmp(argv[argpos], "-h") == 0) {
            usage(argv[0]);
            return -1;
        } else if (strncmp(argv[argpos], "--", 2) == 0) {
            // TODO: check if is valid.
            some_parameter = true;
            options->Set(argv[argpos]+2, Boolean::New(true));
        }
    }

    if (source.IsEmpty()) {
        usage(argv[0]);
        return -1;
    }

    if (!some_parameter) {
        options->Set("bitwise", Boolean::New(true));
        options->Set("eqeqeq", Boolean::New(true));
        options->Set("immed", Boolean::New(true));
        options->Set("newcap", Boolean::New(true));
        options->Set("nomen", Boolean::New(true));
        options->Set("onevar", Boolean::New(true));
        options->Set("regexp", Boolean::New(true));
        options->Set("undef", Boolean::New(true));
        options->Set("white", Boolean::New(true));
    }

    global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));
    global->Set("source", source);
    global->Set("options", options);
    
    Handle<Context> context = Context::New(NULL, global);

    Context::Scope context_scope(context);

    Handle<Script> beautifyScript = Script::Compile(String::New(jslint_code));
    beautifyScript->Run();
    
    Handle<Script> runnerScript = Script::Compile(String::New("JSLINT(source, options);"));
    Handle<Value> result = runnerScript->Run();

    Handle<Script> printScript = Script::Compile(String::New(v8_code));
    printScript->Run();

    return 0;
}

