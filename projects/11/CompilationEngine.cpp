#include "JackAnalyzer.hpp"


string token[5] = {"keyword", "symbol", "identifier", "intConst", "stringConst"};
map<string, Kind> str2kind = {
    {"static", STATIC},
    {"field", FIELD},
    {"argument", ARG},
    {"var", VAR},
    {"class", NONE},
    {"subroutine", NONE},
};
map<Kind, string> kind2str = {
    {STATIC, "static"},
    {FIELD, "field"},
    {ARG, "argument"},
    {VAR, "var"},
    {NONE, "none"},
};
map<string, ArithCommand> str2cmd[2] = {
    {
      {"+", ADD},
      {"-", SUB},
      {"=", EQ},
      {">", GT},
      {"<", LT},
      {"&", AND},
      {"|", OR}
    }, {
      {"-", NEG},
      {"~", NOT}
    }
};
Segment kind2seg[5] = {SEG_STATIC, THIS, SEG_ARG, LOCAL, TEMP};

CompilationEngine::CompilationEngine(string input, string directory, string name) : tokenizer(input), writer(directory, name) {
  // fout.open(directory+name+".xml");
  compileClass();
  // fout.close();
}

void CompilationEngine::raiseError(string type, string expect, string actual) {
  cout << type << " error: expected " << expect << ", but found " << actual << " (in " << writer.getClassName() << ")\n";
  tokenizer.regress();
  cout << "before token: " << token[tokenizer.tokenType()] << "(" << tokenizer.keyword() << ")\n";
  writer.close();
  exit(1);
}

void CompilationEngine::tokenCheck(Token expect) {
  Token actual = tokenizer.tokenType();
  if (expect == actual)
    return ;
  raiseError("token", token[expect], token[actual]+";"+tokenizer.keyword());
}

void CompilationEngine::keywordCheck(string expect) {
  tokenCheck(KEYWORD);
  string actual = tokenizer.keyword();
  if (expect == actual)
    return ;
  raiseError("keyword", expect, actual);
}

void CompilationEngine::symbolCheck(string expect) {
  tokenCheck(SYMBOL);
  string actual = tokenizer.symbol();
  if (expect == actual)
    return;
  raiseError("symbol", expect, actual);
}

string CompilationEngine::compileKeyword() {
  string keyword = tokenizer.keyword();
  // fout << "<keyword> " << keyword << " </keyword>\n";
  if (keyword == "true") {
    writer.writePush(CONST, 1);
    writer.writeArithmetic(NEG);
  } else if (keyword == "false" || keyword == "null") {
    writer.writePush(CONST, 0);
  } else if (keyword == "this") {
    writer.writePush(POINTER, 0);
  }
  tokenizer.advance();
  return keyword;
}

string CompilationEngine::compileSymbol() {
  string sym = tokenizer.symbol();
  // fout << "<symbol> ";
  // if (sym == "<")
  //   fout << "&lt;";
  // else if (sym == ">")
  //   fout << "&gt;";
  // else if (sym == "&")
  //   fout << "&amp;";
  // else
  //   fout << tokenizer.symbol();
  // fout << " </symbol>\n";
  tokenizer.advance();
  return sym;
}

void CompilationEngine::compileInt() {
  // fout << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>\n";
  writer.writePush(CONST, tokenizer.intVal());
  tokenizer.advance();
}

void CompilationEngine::compileString() {
  // fout << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>\n";
  string strVal = tokenizer.stringVal();
  int size = strVal.size();
  writer.writePush(CONST, size+5);
  writer.writeCall("String.new", 1);
  for (int i=0; i<size; i++) {
    writer.writePush(CONST, strVal[i]);
    writer.writeCall("String.appendChar", 2);
  }
  tokenizer.advance();
}

string CompilationEngine::compileIdentifier(string category, string state, string type="") {
  tokenCheck(IDENTIFIER);
  string name = tokenizer.identifier();
  if (state == "defined") {
    Kind k = str2kind[category];
    table.define(name, type, k);
  }
  // int index = -1;
  // Kind kind = table.kindOf(name);
  // if (kind != 4) {
  //   index = table.indexOf(name);
  // }
  // fout << "<identifier> " << name << "(" << category << ", " << state << ", " << kind2str[kind] << ", " << index << ") </identifier>\n";
  tokenizer.advance();
  return name;
}

string CompilationEngine::compileType() {
  if (tokenizer.tokenType() == IDENTIFIER) {
    return compileIdentifier("class", "used");
  }
  tokenCheck(KEYWORD);
  string keyword = tokenizer.keyword();
  if (keyword == "int" || keyword == "char" || keyword == "boolean") {
    return compileKeyword();
  }
  raiseError("keyword", "\"int\" or \"char\" or \"boolean\"", keyword);
  return "";
}

void CompilationEngine::compileClass() {
  // fout << "<class>\n";
  keywordCheck("class");
  compileKeyword();
  string class_name = compileIdentifier("class", "defined");
  writer.setClassName(class_name);
  symbolCheck("{");
  compileSymbol();
  while (tokenizer.checkClassVarDec()) {
    compileClassVarDec();
  }
  while (tokenizer.checkSubroutine()) {
    compileSubroutine();
  }
  symbolCheck("}");
  compileSymbol();
  writer.close();
  // fout << "</class>\n";
}

void CompilationEngine::compileClassVarDec() {
  // fout << "<classVarDec>\n";
  string kind = compileKeyword();
  string type = compileType();
  compileIdentifier(kind, "defined", type);
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileIdentifier(kind, "defined", type);
  }
  symbolCheck(";");
  compileSymbol();
  // fout << "</classVarDec>\n";
}

void CompilationEngine::compileSubroutine() {
  // fout << "<subroutineDec>\n";
  string subroutine_kind = compileKeyword();
  int nLocals = 0;
  table.startSubroutine();
  writer.setIsMethod(subroutine_kind == "method");
  string type;
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyword() == "void")
    type = compileKeyword();
  else
    type = compileType();
  string subroutine_name = compileIdentifier("subroutine", "defined", type);
  symbolCheck("(");
  compileSymbol();
  compileParameterList();
  symbolCheck(")");
  compileSymbol();
  // fout << "<subroutineBody>\n";
  symbolCheck("{");
  compileSymbol();
  while (tokenizer.tokenType() == KEYWORD && tokenizer.keyword() == "var") {
    nLocals += compileVarDec();
  }
  writer.writeFunction(subroutine_name, nLocals);
  if (subroutine_kind == "method") {
    writer.writePush(SEG_ARG, -1);
    writer.writePop(POINTER, 0);
  } else if (subroutine_kind == "constructor") {
    int fields = table.varCount(FIELD);
    writer.writePush(CONST, fields);
    writer.writeCall("Memory.alloc", 1);
    writer.writePop(POINTER, 0);
  }
  compileStatements();
  symbolCheck("}");
  compileSymbol();
  // fout << "</subroutineBody>\n";
  // fout << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
  // fout << "<parameterList>\n";
  if (tokenizer.tokenType() == SYMBOL) {
    // fout << "</parameterList>\n";
    return ;
  }
  string type = compileType();
  compileIdentifier("argument", "defined", type);
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    type = compileType();
    compileIdentifier("argument", "defined", type);
  }
  // fout << "</parameterList>\n";
}

int CompilationEngine::compileVarDec() {
  // fout << "<varDec>\n";
  int nVar = 0;
  string kind = compileKeyword();
  string type = compileType();
  compileIdentifier(kind, "defined", type);
  nVar++;
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileIdentifier(kind, "defined", type);
    nVar++;
  }
  symbolCheck(";");
  compileSymbol();
  return nVar;
  // fout << "</varDec>\n";
}

void CompilationEngine::compileStatements() {
  // fout << "<statements>\n";
  while (tokenizer.tokenType() == KEYWORD) {
    string key = tokenizer.keyword();
    if (key == "let")
      compileLet();
    else if (key == "if")
      compileIf();
    else if (key == "while")
      compileWhile();
    else if (key == "do")
      compileDo();
    else if (key == "return")
      compileReturn();
  }
  // fout << "</statements>\n";
}

void CompilationEngine::compileDo() {
  // fout << "<doStatement>\n";
  compileKeyword();
  compileSubroutineCall();
  writer.writePop(TEMP, 0);
  symbolCheck(";");
  compileSymbol();
  // fout << "</doStatement>\n";
}

void CompilationEngine::compileLet() {
  // fout << "<letStatement>\n";
  compileKeyword();
  string name = compileIdentifier("var", "used");
  Segment seg = kind2seg[table.kindOf(name)];
  if (seg == TEMP) {
    cout << "variable " << name << " isn't defined.\n";
    writer.close();
    exit(1);
  }
  tokenCheck(SYMBOL);
  if (tokenizer.symbol() == "[") {
    compileSymbol();
    writer.writePush(seg, table.indexOf(name));
    compileExpression();
    symbolCheck("]");
    compileSymbol();
    writer.writeArithmetic(ADD);
    writer.writePop(TEMP, 1);
    symbolCheck("=");
    compileSymbol();
    compileExpression();
    symbolCheck(";");
    compileSymbol();
    writer.writePush(TEMP, 1);
    writer.writePop(POINTER, 1);
    writer.writePop(THAT, 0);
  } else {
    symbolCheck("=");
    compileSymbol();
    compileExpression();
    symbolCheck(";");
    compileSymbol();
    writer.writePop(seg, table.indexOf(name));
  }
  // fout << "</letStatement>\n";
}

void CompilationEngine::compileWhile() {
  // fout << "<whileStatement>\n";
  int id = while_label++;
  compileKeyword();
  writer.writeLabel("WHILE_EXP"+to_string(id));
  symbolCheck("(");
  compileSymbol();
  compileExpression();
  symbolCheck(")");
  writer.writeArithmetic(NOT);
  writer.writeIf("WHILE_END"+to_string(id));
  compileSymbol();
  symbolCheck("{");
  compileSymbol();
  compileStatements();
  symbolCheck("}");
  compileSymbol();
  writer.writeGoto("WHILE_EXP"+to_string(id));
  writer.writeLabel("WHILE_END"+to_string(id));
  // fout << "</whileStatement>\n";
}

void CompilationEngine::compileReturn() {
  // fout << "<returnStatement>\n";
  compileKeyword();
  if (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ";")) {
    compileExpression();
  } else {
    writer.writePush(CONST, 0);
  }
  symbolCheck(";");
  compileSymbol();
  writer.writeReturn();
  // fout << "</returnStatement>\n";
}

void CompilationEngine::compileIf() {
  // fout << "<ifStatement>\n";
  int id = if_label++;
  compileKeyword();
  symbolCheck("(");
  compileSymbol();
  compileExpression();
  symbolCheck(")");
  compileSymbol();
  writer.writeArithmetic(NOT);
  writer.writeIf("IF_FALSE"+to_string(id));
  symbolCheck("{");
  compileSymbol();
  compileStatements();
  symbolCheck("}");
  writer.writeGoto("IF_TRUE"+to_string(id));
  writer.writeLabel("IF_FALSE"+to_string(id));
  compileSymbol();
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyword() == "else") {
    compileKeyword();
    symbolCheck("{");
    compileSymbol();
    compileStatements();
    symbolCheck("}");
    compileSymbol();
  }
  writer.writeLabel("IF_TRUE"+to_string(id));
  // fout << "</ifStatement>\n";
}

void CompilationEngine::compileExpression() {
  // fout << "<expression>\n";
  compileTerm();
  while (tokenizer.checkOp()) {
    string op = compileSymbol();
    compileTerm();
    if (op == "*") {
      writer.writeCall("Math.multiply",2);
    } else if (op == "/") {
      writer.writeCall("Math.divide", 2);
    } else {
      writer.writeArithmetic(str2cmd[0][op]);
    }
  }
  // fout << "</expression>\n";
}

void CompilationEngine::compileTerm() {
  // fout << "<term>\n";
  Token type = tokenizer.tokenType();
  if (type == INT_CONST) {
    compileInt();
  } else if (type == STRING_CONST) {
    compileString();
  } else if (type == KEYWORD) {
    if (tokenizer.checkKeywordConstant()) {
      compileKeyword();
    } else {
      raiseError("keyword", "constant keyword (\"true\", \"false\", \"null\", \"this\")", tokenizer.keyword());
    }
  } else if (type == IDENTIFIER) {
    tokenizer.advance();
    if (tokenizer.tokenType() == SYMBOL && (tokenizer.symbol() == "." || tokenizer.symbol() == "(")) {
      tokenizer.regress();
      compileSubroutineCall();
    } else {
      tokenizer.regress();
      string name = compileIdentifier("var", "used");
      Segment seg = kind2seg[table.kindOf(name)];
      if (seg == TEMP) {
        cout << "variable " << name << " isn't defined.\n";
        writer.close();
        exit(1);
      }
      writer.writePush(seg, table.indexOf(name));
      if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == "[") {
        compileSymbol();
        compileExpression();
        symbolCheck("]");
        compileSymbol();
        writer.writeArithmetic(ADD);
        writer.writePop(POINTER, 1);
        writer.writePush(THAT,0);
      }
    }
  } else if (type == SYMBOL) {
    if (tokenizer.symbol() == "(") {
      compileSymbol();
      compileExpression();
      symbolCheck(")");
      compileSymbol();
    } else if (tokenizer.checkUnaryOp()) {
      string unaryOp = compileSymbol();
      compileTerm();
      writer.writeArithmetic(str2cmd[1][unaryOp]);
    } else {
      raiseError("symbol", "unary op (\"-\", \"~\") or \"(\"", tokenizer.symbol());
    }
  }
  // fout << "</term>\n";
}

int CompilationEngine::compileExpressionList() {
  // fout << "<expressionList>\n";
  int nArgs = 0;
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ")") {
    // fout << "</expressionList>\n";
    return nArgs;
  }
  compileExpression();
  nArgs++;
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileExpression();
    nArgs++;
  }
  // fout << "</expressionList>\n";
  return nArgs;
}

void CompilationEngine::compileSubroutineCall() {
  int nArgs = 0;
  string subroutine_name = writer.getClassName();
  tokenizer.advance();
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ".") {
    tokenizer.regress();
    string name = tokenizer.identifier();
    if (table.kindOf(name) == NONE)
      subroutine_name = compileIdentifier("class", "used");
    else {
      compileIdentifier("var", "used");
      subroutine_name = table.typeOf(name);
      Segment seg = kind2seg[table.kindOf(name)];
      if (seg == TEMP) {
        cout << "variable " << name << " isn't defined.\n";
        writer.close();
        exit(1);
      }
      writer.writePush(seg, table.indexOf(name));
      nArgs++;
    }
    compileSymbol();
  } else {
    writer.writePush(POINTER, 0);
    nArgs++;
    tokenizer.regress();
  }
  subroutine_name.push_back('.');
  subroutine_name += compileIdentifier("subroutine", "used");
  symbolCheck("(");
  compileSymbol();
  nArgs += compileExpressionList();
  symbolCheck(")");
  compileSymbol();
  writer.writeCall(subroutine_name, nArgs);
}
