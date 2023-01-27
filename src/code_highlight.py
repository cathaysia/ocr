# -*- coding: utf-8 -*-
from pygments import highlight
from pygments.formatters import HtmlFormatter
from pygments.lexers import guess_lexer


def code_shader(content: str) -> str:
    lexer = guess_lexer(content)
    return highlight(content, lexer, HtmlFormatter(noclasses=True, style="one-dark"))
