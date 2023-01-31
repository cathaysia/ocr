# -*- coding: utf-8 -*-
from typing import List


def code_shader(content: str, theme: str) -> str:
    from pygments import highlight
    from pygments.formatters import HtmlFormatter
    from pygments.lexers import guess_lexer

    lexer = guess_lexer(content)
    return highlight(content, lexer, HtmlFormatter(noclasses=True, style=theme))


def available_themes() -> List[str]:
    from pygments.styles import STYLE_MAP

    return list(STYLE_MAP.keys())
