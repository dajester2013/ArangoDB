static string JS_common_bootstrap_modules = 
  "/*jslint indent: 2,\n"
  "         nomen: true,\n"
  "         maxlen: 100,\n"
  "         sloppy: true,\n"
  "         vars: true,\n"
  "         white: true,\n"
  "         plusplus: true */\n"
  "/*global require, module, ModuleCache, SYS_EXECUTE, CONSOLE_ERROR,\n"
  " FS_MOVE, FS_REMOVE, FS_EXISTS, \n"
  " SYS_LOAD, SYS_LOG, SYS_LOG_LEVEL, SYS_OUTPUT,\n"
  " SYS_PROCESS_STAT, SYS_READ, SYS_SPRINTF, SYS_TIME,\n"
  " SYS_START_PAGER, SYS_STOP_PAGER, ARANGO_QUIET, MODULES_PATH,\n"
  " COLOR_OUTPUT, COLOR_OUTPUT_RESET, COLOR_BRIGHT, PRETTY_PRINT,\n"
  " SYS_SHA256, SYS_WAIT, SYS_GETLINE */\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief JavaScript server functions\n"
  "///\n"
  "/// @file\n"
  "///\n"
  "/// DISCLAIMER\n"
  "///\n"
  "/// Copyright 2010-2011 triagens GmbH, Cologne, Germany\n"
  "///\n"
  "/// Licensed under the Apache License, Version 2.0 (the \"License\");\n"
  "/// you may not use this file except in compliance with the License.\n"
  "/// You may obtain a copy of the License at\n"
  "///\n"
  "///     http://www.apache.org/licenses/LICENSE-2.0\n"
  "///\n"
  "/// Unless required by applicable law or agreed to in writing, software\n"
  "/// distributed under the License is distributed on an \"AS IS\" BASIS,\n"
  "/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
  "/// See the License for the specific language governing permissions and\n"
  "/// limitations under the License.\n"
  "///\n"
  "/// Copyright holder is triAGENS GmbH, Cologne, Germany\n"
  "///\n"
  "/// @author Dr. Frank Celler\n"
  "/// @author Copyright 2011, triAGENS GmbH, Cologne, Germany\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "// -----------------------------------------------------------------------------\n"
  "// --SECTION--                                                            Module\n"
  "// -----------------------------------------------------------------------------\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @addtogroup V8Module\n"
  "/// @{\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief module cache\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "ModuleCache = {};\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief file exists cache\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "ExistsCache = {};\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief module constructor\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "function Module (id) {\n"
  "  this.id = id;\n"
  "  this.exports = {};\n"
  "}\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief loads a file and creates a new module descriptor\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "Module.prototype.require = function (path) {\n"
  "  var content;\n"
  "  var f;\n"
  "  var module;\n"
  "  var paths;\n"
  "  var raw;\n"
  "  var sandbox;\n"
  "\n"
  "  // first get rid of any \"..\" and \".\"\n"
  "  path = this.normalise(path);\n"
  "\n"
  "  // check if you already know the module, return the exports\n"
  "  if (ModuleCache.hasOwnProperty(path)) {\n"
  "    return ModuleCache[path].exports;\n"
  "  }\n"
  "\n"
  "  // locate file and read content\n"
  "  raw = ModuleCache[\"/internal\"].exports.readFile(path);\n"
  "\n"
  "  // test for parse errors first and fail early if a parse error detected\n"
  "  if (! SYS_PARSE(raw.content, path)) {\n"
  "    throw \"Javascript parse error in file '\" + path + \"'\";\n"
  "  }\n"
  "\n"
  "  // create a new sandbox and execute\n"
  "  module = ModuleCache[path] = new Module(path);\n"
  "\n"
  "  content = \"(function (module, exports, require, print) {\"\n"
  "          + raw.content \n"
  "          + \"\\n});\";\n"
  "  \n"
  "  f = SYS_EXECUTE(content, undefined, path);\n"
  "\n"
  "  if (f === undefined) {\n"
  "    throw \"cannot create context function\";\n"
  "  }\n"
  " \n"
  "  try {\n"
  "    f(module,\n"
  "      module.exports,\n"
  "      function(path) { return module.require(path); },\n"
  "      ModuleCache[\"/internal\"].exports.print);\n"
  "  }\n"
  "  catch (err) {\n"
  "    throw \"Javascript exception in file '\" + path + \"': \" + err.stack;\n"
  "  }\n"
  "\n"
  "  return module.exports;\n"
  "};\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief normalises a path\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "Module.prototype.normalise = function (path) {\n"
  "  var i;\n"
  "  var n;\n"
  "  var p;\n"
  "  var q;\n"
  "  var x;\n"
  "\n"
  "  if (path === \"\") {\n"
  "    return this.id;\n"
  "  }\n"
  "\n"
  "  p = path.split('/');\n"
  "\n"
  "  // relative path\n"
  "  if (p[0] === \".\" || p[0] === \"..\") {\n"
  "    q = this.id.split('/');\n"
  "    q.pop();\n"
  "    q = q.concat(p);\n"
  "  }\n"
  "\n"
  "  // absolute path\n"
  "  else {\n"
  "    q = p;\n"
  "  }\n"
  "\n"
  "  // normalize path\n"
  "  n = [];\n"
  "\n"
  "  for (i = 0;  i < q.length;  ++i) {\n"
  "    x = q[i];\n"
  "\n"
  "    if (x === \"..\") {\n"
  "      if (n.length === 0) {\n"
  "        throw \"cannot cross module top\";\n"
  "      }\n"
  "\n"
  "      n.pop();\n"
  "    }\n"
  "    else if (x !== \"\" && x !== \".\") {\n"
  "      n.push(x);\n"
  "    }\n"
  "  }\n"
  "\n"
  "  return \"/\" + n.join('/');\n"
  "};\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief unloads module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "Module.prototype.unload = function (path) {\n"
  "  if (! path) {\n"
  "    return;\n"
  "  }\n"
  "\n"
  "  var norm = module.normalise(path);\n"
  "\n"
  "  if (   norm === \"/\"\n"
  "      || norm === \"/internal\"\n"
  "      || norm === \"/console\"\n"
  "      || norm === \"/fs\") {\n"
  "    return;\n"
  "  }\n"
  "\n"
  "  delete ModuleCache[norm];\n"
  "};\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief unloads module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "Module.prototype.unloadAll = function () {\n"
  "  var path;\n"
  "\n"
  "  for (path in ModuleCache) {\n"
  "    if (ModuleCache.hasOwnProperty(path)) {\n"
  "      this.unload(path);\n"
  "    }\n"
  "  }\n"
  "};\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief top-level module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "module = ModuleCache[\"/\"] = new Module(\"/\");\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief global require function\n"
  "///\n"
  "/// @FUN{require(@FA{path})}\n"
  "///\n"
  "/// @FN{require} checks if the file specified by @FA{path} has already been\n"
  "/// loaded.  If not, the content of the file is executed in a new\n"
  "/// context. Within the context you can use the global variable @CODE{exports}\n"
  "/// in order to export variables and functions. This variable is returned by\n"
  "/// @FN{require}.\n"
  "///\n"
  "/// Assume that your module file is @CODE{test1.js} and contains\n"
  "///\n"
  "/// @verbinclude modules-require-1\n"
  "///\n"
  "/// Then you can use @FN{require} to load the file and access the exports.\n"
  "///\n"
  "/// @verbinclude modules-require-2\n"
  "///\n"
  "/// @FN{require} follows the specification\n"
  "/// <a href=\"http://wiki.commonjs.org/wiki/Modules/1.1.1\">Modules/1.1.1</a>.\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "function require (path) {\n"
  "  return module.require(path);\n"
  "}\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @}\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "// -----------------------------------------------------------------------------\n"
  "// --SECTION--                                                       Module \"fs\"\n"
  "// -----------------------------------------------------------------------------\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @addtogroup V8ModuleFS\n"
  "/// @{\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief file-system module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "ModuleCache[\"/fs\"] = new Module(\"/fs\");\n"
  "\n"
  "(function () {\n"
  "  var fs = ModuleCache[\"/fs\"].exports;\n"
  "\n"
  "  fs.exists = FS_EXISTS;\n"
  "  fs.move = FS_MOVE;\n"
  "  fs.remove = FS_REMOVE;\n"
  "}());\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @}\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "// -----------------------------------------------------------------------------\n"
  "// --SECTION--                                                 Module \"internal\"\n"
  "// -----------------------------------------------------------------------------\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @addtogroup V8ModuleInternal\n"
  "/// @{\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief internal module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "ModuleCache[\"/internal\"] = new Module(\"/internal\");\n"
  "\n"
  "(function () {\n"
  "  var internal = ModuleCache[\"/internal\"].exports;\n"
  "  var fs = ModuleCache[\"/fs\"].exports;\n"
  "\n"
  "  // system functions\n"
  "  internal.execute = SYS_EXECUTE;\n"
  "  internal.load = SYS_LOAD;\n"
  "  internal.log = SYS_LOG;\n"
  "  internal.logLevel = SYS_LOG_LEVEL;\n"
  "  internal.output = SYS_OUTPUT;\n"
  "  internal.processStat = SYS_PROCESS_STAT;\n"
  "  internal.read = SYS_READ;\n"
  "  internal.sprintf = SYS_SPRINTF;\n"
  "  internal.time = SYS_TIME;\n"
  "  internal.sha256 = SYS_SHA256;\n"
  "  internal.wait = SYS_WAIT;\n"
  "\n"
  "\n"
  "  // command line parameter\n"
  "  internal.MODULES_PATH = \"\";\n"
  "\n"
  "  if (typeof MODULES_PATH !== \"undefined\") {\n"
  "    internal.MODULES_PATH = MODULES_PATH;\n"
  "  }\n"
  "\n"
  "\n"
  "  // output \n"
  "  internal.start_pager = function() {};\n"
  "  internal.stop_pager = function() {};\n"
  "\n"
  "  internal.ARANGO_QUIET = false;\n"
  "\n"
  "  internal.COLOR_OUTPUT = undefined;\n"
  "  internal.COLOR_OUTPUT_RESET = \"\";\n"
  "  internal.COLOR_BRIGHT = \"\";\n"
  "\n"
  "  internal.PRETTY_PRINT = false;\n"
  "\n"
  "  if (typeof SYS_START_PAGER !== \"undefined\") {\n"
  "    internal.start_pager = SYS_START_PAGER;\n"
  "  }\n"
  "\n"
  "  if (typeof SYS_STOP_PAGER !== \"undefined\") {\n"
  "    internal.stop_pager = SYS_STOP_PAGER;\n"
  "  }\n"
  "\n"
  "  if (typeof COLOR_OUTPUT !== \"undefined\") {\n"
  "    internal.COLOR_OUTPUT = COLOR_OUTPUT;\n"
  "  }\n"
  "\n"
  "  if (typeof COLOR_OUTPUT_RESET !== \"undefined\") {\n"
  "    internal.COLOR_OUTPUT_RESET = COLOR_OUTPUT_RESET;\n"
  "  }\n"
  "\n"
  "  if (typeof COLOR_BRIGHT !== \"undefined\") {\n"
  "    internal.COLOR_BRIGHT = COLOR_BRIGHT;\n"
  "  }\n"
  "\n"
  "  if (typeof PRETTY_PRINT !== \"undefined\") {\n"
  "    internal.PRETTY_PRINT = PRETTY_PRINT;\n"
  "  }\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief reads a file from the module path or the database\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  internal.readFile = function (path) {\n"
  "    var i;\n"
  "    var mc;\n"
  "    var n;\n"
  "\n"
  "    var existsCache = ExistsCache;\n"
  "\n"
  "    // try to load the file\n"
  "    var paths = internal.MODULES_PATH;\n"
  "\n"
  "    for (i = 0;  i < paths.length;  ++i) {\n"
  "      var p = paths[i];\n"
  "\n"
  "      if (p === \"\") {\n"
  "        n = \".\" + path + \".js\";\n"
  "      }\n"
  "      else {\n"
  "        n = p + \"/\" + path + \".js\";\n"
  "      }\n"
  "\n"
  "      if (fs.exists(n)) {\n"
  "        existsCache[path] = true;\n"
  "        return { path : n, content : internal.read(n) };\n"
  "      }\n"
  "    }\n"
  "\n"
  "    // try to load the module from the database\n"
  "    mc = internal.db._collection(\"_modules\");\n"
  "\n"
  "    if (mc !== null && (\"firstExample\" in mc)) {\n"
  "      n = mc.firstExample({ path: path });\n"
  "\n"
  "      if (n !== null) {\n"
  "        if (n.hasOwnProperty('content')) {\n"
  "          existsCache[path] = true;\n"
  "          return { path : \"_collection/\" + path, content : n.content };\n"
  "        }\n"
  "        else {\n"
  "	  require(\"console\").error(\"found empty content in '%s'\", JSON.stringify(n));\n"
  "        }\n"
  "      }\n"
  "    }\n"
  "\n"
  "    existsCache[path] = false;\n"
  "    throw \"cannot find a file named '\"\n"
  "        + path\n"
  "        + \"' using the module path(s) '\" \n"
  "        + internal.MODULES_PATH + \"'\";\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief loads a file from the file-system\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  internal.loadFile = function (path) {\n"
  "    var i;\n"
  "\n"
  "    // try to load the file\n"
  "    var paths = internal.MODULES_PATH;\n"
  "\n"
  "    for (i = 0;  i < paths.length;  ++i) {\n"
  "      var p = paths[i];\n"
  "      var n;\n"
  "\n"
  "      if (p === \"\") {\n"
  "        n = \".\" + path + \".js\";\n"
  "      }\n"
  "      else {\n"
  "        n = p + \"/\" + path + \".js\";\n"
  "      }\n"
  "\n"
  "      if (fs.exists(n)) {\n"
  "        return internal.load(n);\n"
  "      }\n"
  "    }\n"
  "\n"
  "    throw \"cannot find a file named '\"\n"
  "        + path \n"
  "        + \"' using the module path(s) '\" \n"
  "        + internal.MODULES_PATH + \"'\";\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief defines a module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  internal.defineModule = function (path, file) {\n"
  "    var content;\n"
  "    var m;\n"
  "    var mc;\n"
  "\n"
  "    content = internal.read(file);\n"
  "\n"
  "    mc = internal.db._collection(\"_modules\");\n"
  "\n"
  "    if (mc === null) {\n"
  "      mc = internal.db._create(\"_modules\", { isSystem: true });\n"
  "    }\n"
  "\n"
  "    path = module.normalise(path);\n"
  "    m = mc.firstExample({ path: path });\n"
  "\n"
  "    if (m === null) {\n"
  "      mc.save({ path: path, module: content });\n"
  "    }\n"
  "    else {\n"
  "      m.module = content;\n"
  "      mc.replace(m, m);\n"
  "    }\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @}\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "}());\n"
  "\n"
  "// -----------------------------------------------------------------------------\n"
  "// --SECTION--                                                  Module \"console\"\n"
  "// -----------------------------------------------------------------------------\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @addtogroup V8ModuleConsole\n"
  "/// @{\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief console module\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "ModuleCache[\"/console\"] = new Module(\"/console\");\n"
  "\n"
  "(function () {\n"
  "  var internal = ModuleCache[\"/internal\"].exports;\n"
  "  var console = ModuleCache[\"/console\"].exports;\n"
  "\n"
  "  console.getline = SYS_GETLINE;\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief logs debug message\n"
  "///\n"
  "/// @FUN{console.debug(@FA{format}, @FA{argument1}, ...)}\n"
  "///\n"
  "/// Formats the arguments according to @FA{format} and logs the result as\n"
  "/// debug message.\n"
  "///\n"
  "/// String substitution patterns, which can be used in @FA{format}.\n"
  "///\n"
  "/// - @LIT{\\%s} string\n"
  "/// - @LIT{\\%d}, @LIT{\\%i} integer\n"
  "/// - @LIT{\\%f} floating point number\n"
  "/// - @LIT{\\%o} object hyperlink\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  console.debug = function () {\n"
  "    var msg;\n"
  "\n"
  "    try {\n"
  "      msg = internal.sprintf.apply(internal.sprintf, arguments);\n"
  "    }\n"
  "    catch (err) {\n"
  "      msg = err + \": \" + arguments;\n"
  "    }\n"
  "\n"
  "    internal.log(\"debug\", msg);\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief logs error message\n"
  "///\n"
  "/// @FUN{console.error(@FA{format}, @FA{argument1}, ...)}\n"
  "///\n"
  "/// Formats the arguments according to @FA{format} and logs the result as\n"
  "/// error message.\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  console.error = function () {\n"
  "    var msg;\n"
  "\n"
  "    try {\n"
  "      msg = internal.sprintf.apply(internal.sprintf, arguments);\n"
  "    }\n"
  "    catch (err) {\n"
  "      msg = err + \": \" + arguments;\n"
  "    }\n"
  "\n"
  "    internal.log(\"error\", msg);\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief logs info message\n"
  "///\n"
  "/// @FUN{console.info(@FA{format}, @FA{argument1}, ...)}\n"
  "///\n"
  "/// Formats the arguments according to @FA{format} and logs the result as\n"
  "/// info message.\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  console.info = function () {\n"
  "    var msg;\n"
  "\n"
  "    try {\n"
  "      msg = internal.sprintf.apply(internal.sprintf, arguments);\n"
  "    }\n"
  "    catch (err) {\n"
  "      msg = err + \": \" + arguments;\n"
  "    }\n"
  "\n"
  "    internal.log(\"info\", msg);\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief logs log message\n"
  "///\n"
  "/// @FUN{console.log(@FA{format}, @FA{argument1}, ...)}\n"
  "///\n"
  "/// Formats the arguments according to @FA{format} and logs the result as\n"
  "/// log message.\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  console.log = function () {\n"
  "    var msg;\n"
  "\n"
  "    try {\n"
  "      msg = internal.sprintf.apply(internal.sprintf, arguments);\n"
  "    }\n"
  "    catch (err) {\n"
  "      msg = err + \": \" + arguments;\n"
  "    }\n"
  "\n"
  "    internal.log(\"info\", msg);\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @brief logs warn message\n"
  "///\n"
  "/// @FUN{console.warn(@FA{format}, @FA{argument1}, ...)}\n"
  "///\n"
  "/// Formats the arguments according to @FA{format} and logs the result as\n"
  "/// warn message.\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "  console.warn = function () {\n"
  "    var msg;\n"
  "\n"
  "    try {\n"
  "      msg = internal.sprintf.apply(internal.sprintf, arguments);\n"
  "    }\n"
  "    catch (err) {\n"
  "      msg = err + \": \" + arguments;\n"
  "    }\n"
  "\n"
  "    internal.log(\"warn\", msg);\n"
  "  };\n"
  "\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "/// @}\n"
  "////////////////////////////////////////////////////////////////////////////////\n"
  "\n"
  "}());\n"
  "\n"
  "// -----------------------------------------------------------------------------\n"
  "// --SECTION--                                                       END-OF-FILE\n"
  "// -----------------------------------------------------------------------------\n"
  "\n"
  "// Local Variables:\n"
  "// mode: outline-minor\n"
  "// outline-regexp: \"^\\\\(/// @brief\\\\|/// @addtogroup\\\\|// --SECTION--\\\\|/// @page\\\\|/// @}\\\\)\"\n"
  "// End:\n"
;