#include "JackAnalyzer.hpp"


string token[5] = {"keyword", "symbol", "identifier", "intConst", "stringConst"};

CompilationEngine::CompilationEngine(string input, string output) : tokenizer(input) {
  fout.open(output);
  compileClass();
  fout.close();
}

void CompilationEngine::raiseError(string type, string expect, string actual) {
  cout << type << " error: expected " << expect << ", but found " << actual << endl;
  fout.close();
  exit(1);
}

void CompilationEngine::tokenCheck(Token expect) {
  Token actual = tokenizer.tokenType();
  if (expect == actual)
    return ;
  raiseError("token", token[expect], token[actual]);
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

void CompilationEngine::compileKeyword() {
  fout << "<keyword> " << tokenizer.keyword() << " </keyword>\n";
  tokenizer.advance();
}

void CompilationEngine::compileSymbol() {
  string sym = tokenizer.symbol();
  fout << "<symbol> ";
  if (sym == "<")
    fout << "&lt;";
  else if (sym == ">")
    fout << "&gt;";
  else if (sym == "&")
    fout << "&amp;";
  else
    fout << tokenizer.symbol();
  fout << " </symbol>\n";
  tokenizer.advance();
}

void CompilationEngine::compileInt() {
  fout << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>\n";
  tokenizer.advance();
}

void CompilationEngine::compileString() {
 fout << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>\n";
 tokenizer.advance();
}

void CompilationEngine::compileIdentifier() {
  tokenCheck(IDENTIFIER);
  fout << "<identifier> " << tokenizer.identifier() << " </identifier>\n";
  tokenizer.advance();
}

void CompilationEngine::compileType() {
  if (tokenizer.tokenType() == IDENTIFIER) {
    compileIdentifier();
    return ;
  }
  tokenCheck(KEYWORD);
  string keyword = tokenizer.keyword();
  if (keyword == "int" || keyword == "char" || keyword == "boolean") {
    compileKeyword();
    return ;
  }
  raiseError("keyword", "\"int\" or \"char\" or \"boolean\"", keyword);
}

void CompilationEngine::compileClass() {
  fout << "<class>\n";
  keywordCheck("class");
  compileKeyword();
  compileIdentifier();
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
  fout << "</class>\n";
}

void CompilationEngine::compileClassVarDec() {
  fout << "<classVarDec>\n";
  compileKeyword();
  compileType();
  compileIdentifier();
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileIdentifier();
  }
  symbolCheck(";");
  compileSymbol();
  fout << "</classVarDec>\n";
}

void CompilationEngine::compileSubroutine() {
  fout << "<subroutineDec>\n";
  compileKeyword();
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyword() == "void")
    compileKeyword();
  else
    compileType();
  compileIdentifier();
  symbolCheck("(");
  compileSymbol();
  compileParameterList();
  symbolCheck(")");
  compileSymbol();
  fout << "<subroutineBody>\n";
  symbolCheck("{");
  compileSymbol();
  while (tokenizer.tokenType() == KEYWORD && tokenizer.keyword() == "var") {
    compileVarDec();
  }
  compileStatements();
  symbolCheck("}");
  compileSymbol();
  fout << "</subroutineBody>\n";
  fout << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
  fout << "<parameterList>\n";
  if (tokenizer.tokenType() == SYMBOL) {
    fout << "</parameterList>\n";
    return ;
  }
  compileType();
  compileIdentifier();
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileType();
    compileIdentifier();
  }
  fout << "</parameterList>\n";
}

void CompilationEngine::compileVarDec() {
  fout << "<varDec>\n";
  compileKeyword();
  compileType();
  compileIdentifier();
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileIdentifier();
  }
  symbolCheck(";");
  compileSymbol();
  fout << "</varDec>\n";
}

void CompilationEngine::compileStatements() {
  fout << "<statements>\n";
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
  fout << "</statements>\n";
}

void CompilationEngine::compileDo() {
  fout << "<doStatement>\n";
  compileKeyword();
  compileIdentifier();
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ".") {
    compileSymbol();
    compileIdentifier();
  }
  symbolCheck("(");
  compileSymbol();
  compileExpressionList();
  symbolCheck(")");
  compileSymbol();
  symbolCheck(";");
  compileSymbol();
  fout << "</doStatement>\n";
}

void CompilationEngine::compileLet() {
  fout << "<letStatement>\n";
  compileKeyword();
  compileIdentifier();
  tokenCheck(SYMBOL);
  if (tokenizer.symbol() == "[") {
    compileSymbol();
    compileExpression();
    symbolCheck("]");
    compileSymbol();
  }
  symbolCheck("=");
  compileSymbol();
  compileExpression();
  symbolCheck(";");
  compileSymbol();
  fout << "</letStatement>\n";
}

void CompilationEngine::compileWhile() {
  fout << "<whileStatement>\n";
  compileKeyword();
  symbolCheck("(");
  compileSymbol();
  compileExpression();
  symbolCheck(")");
  compileSymbol();
  symbolCheck("{");
  compileSymbol();
  compileStatements();
  symbolCheck("}");
  compileSymbol();
  fout << "</whileStatement>\n";
}

void CompilationEngine::compileReturn() {
  fout << "<returnStatement>\n";
  compileKeyword();
  if (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ";")) {
    compileExpression();
  }
  symbolCheck(";");
  compileSymbol();
  fout << "</returnStatement>\n";
}

void CompilationEngine::compileIf() {
  fout << "<ifStatement>\n";
  compileKeyword();
  symbolCheck("(");
  compileSymbol();
  compileExpression();
  symbolCheck(")");
  compileSymbol();
  symbolCheck("{");
  compileSymbol();
  compileStatements();
  symbolCheck("}");
  compileSymbol();
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyword() == "else") {
    compileKeyword();
    symbolCheck("{");
    compileSymbol();
    compileStatements();
    symbolCheck("}");
    compileSymbol();
  }
  fout << "</ifStatement>\n";
}

void CompilationEngine::compileExpression() {
  fout << "<expression>\n";
  compileTerm();
  while (tokenizer.checkOp()) {
    compileSymbol();
    compileTerm();
  }
  fout << "</expression>\n";
}

void CompilationEngine::compileTerm() {
  fout << "<term>\n";
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
    compileIdentifier();
    Token type_next = tokenizer.tokenType();
    if (type_next == SYMBOL) {
      if (tokenizer.symbol() == "[") {
        compileSymbol();
        compileExpression();
        symbolCheck("]");
        compileSymbol();
      } else {
        if (tokenizer.symbol() == ".") {
          compileSymbol();
          compileIdentifier();
          symbolCheck("(");
        }
        if (tokenizer.symbol() == "(") {
          compileSymbol();
          compileExpressionList();
          symbolCheck(")");
          compileSymbol();
        }
      }
    }
  } else if (type == SYMBOL) {
    if (tokenizer.symbol() == "(") {
      compileSymbol();
      compileExpression();
      symbolCheck(")");
      compileSymbol();
    } else if (tokenizer.checkUnaryOp()) {
      compileSymbol();
      compileTerm();
    } else {
      raiseError("symbol", "unary op (\"-\", \"~\") or \"(\"", tokenizer.symbol());
    }
  }
  fout << "</term>\n";
}

void CompilationEngine::compileExpressionList() {
  fout << "<expressionList>\n";
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ")") {
    fout << "</expressionList>\n";
    return ;
  }
  compileExpression();
  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ",") {
    compileSymbol();
    compileExpression();
  }
  fout << "</expressionList>\n";
}