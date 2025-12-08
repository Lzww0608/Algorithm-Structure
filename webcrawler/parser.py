"""
Content Parser - 内容解析器

功能:
1. 验证和解析 HTML 内容
2. 将 HTML 转换为可操作的 DOM 结构
3. 提取页面元数据（标题、描述等）
"""

import re
from typing import Optional, List, Dict, Any
from dataclasses import dataclass, field
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse


@dataclass
class ParsedContent:
    """解析后的内容"""
    url: str                                    # 原始 URL
    success: bool                               # 解析是否成功
    title: str = ""                             # 页面标题
    description: str = ""                       # 页面描述
    text_content: str = ""                      # 纯文本内容
    links: List[str] = field(default_factory=list)          # 所有链接
    images: List[str] = field(default_factory=list)         # 所有图片 URL
    scripts: List[str] = field(default_factory=list)        # 所有脚本 URL
    styles: List[str] = field(default_factory=list)         # 所有样式表 URL
    meta_tags: Dict[str, str] = field(default_factory=dict) # Meta 标签
    headings: Dict[str, List[str]] = field(default_factory=dict)  # 标题（h1-h6）
    word_count: int = 0                         # 字数统计
    error_message: str = ""                     # 错误信息


class ContentParser:
    """
    HTML 内容解析器
    
    使用 BeautifulSoup 解析 HTML 并提取结构化数据
    """
    
    def __init__(self, parser_type: str = "lxml"):
        """
        初始化解析器
        
        Args:
            parser_type: BeautifulSoup 解析器类型 (lxml, html.parser, html5lib)
        """
        self._parser_type = parser_type
    
    def parse(self, html: str, base_url: str = "") -> ParsedContent:
        """
        解析 HTML 内容
        
        Args:
            html: HTML 字符串
            base_url: 基础 URL（用于解析相对链接）
            
        Returns:
            ParsedContent 对象
        """
        result = ParsedContent(url=base_url, success=False)
        
        if not html or not html.strip():
            result.error_message = "空内容"
            return result
        
        try:
            soup = BeautifulSoup(html, self._parser_type)
            
            # 提取标题
            result.title = self._extract_title(soup)
            
            # 提取描述
            result.description = self._extract_description(soup)
            
            # 提取 Meta 标签
            result.meta_tags = self._extract_meta_tags(soup)
            
            # 提取纯文本
            result.text_content = self._extract_text(soup)
            result.word_count = len(result.text_content.split())
            
            # 提取链接
            result.links = self._extract_links(soup, base_url)
            
            # 提取图片
            result.images = self._extract_images(soup, base_url)
            
            # 提取脚本和样式
            result.scripts = self._extract_scripts(soup, base_url)
            result.styles = self._extract_styles(soup, base_url)
            
            # 提取标题层级
            result.headings = self._extract_headings(soup)
            
            result.success = True
            
        except Exception as e:
            result.error_message = f"解析错误: {str(e)}"
        
        return result
    
    def _extract_title(self, soup: BeautifulSoup) -> str:
        """提取页面标题"""
        # 优先从 <title> 标签获取
        title_tag = soup.find("title")
        if title_tag:
            return title_tag.get_text(strip=True)
        
        # 其次从 og:title 获取
        og_title = soup.find("meta", property="og:title")
        if og_title:
            return og_title.get("content", "")
        
        # 最后从 h1 获取
        h1_tag = soup.find("h1")
        if h1_tag:
            return h1_tag.get_text(strip=True)
        
        return ""
    
    def _extract_description(self, soup: BeautifulSoup) -> str:
        """提取页面描述"""
        # 优先从 meta description 获取
        meta_desc = soup.find("meta", attrs={"name": "description"})
        if meta_desc:
            return meta_desc.get("content", "")
        
        # 其次从 og:description 获取
        og_desc = soup.find("meta", property="og:description")
        if og_desc:
            return og_desc.get("content", "")
        
        return ""
    
    def _extract_meta_tags(self, soup: BeautifulSoup) -> Dict[str, str]:
        """提取所有 Meta 标签"""
        meta_tags = {}
        for meta in soup.find_all("meta"):
            name = meta.get("name") or meta.get("property") or meta.get("http-equiv")
            content = meta.get("content")
            if name and content:
                meta_tags[name] = content
        return meta_tags
    
    def _extract_text(self, soup: BeautifulSoup) -> str:
        """提取纯文本内容"""
        # 移除脚本和样式
        for tag in soup(["script", "style", "noscript", "nav", "footer", "header"]):
            tag.decompose()
        
        # 获取文本
        text = soup.get_text(separator=" ", strip=True)
        
        # 清理多余空白
        text = re.sub(r'\s+', ' ', text)
        
        return text.strip()
    
    def _extract_links(self, soup: BeautifulSoup, base_url: str) -> List[str]:
        """提取所有链接"""
        links = []
        for a_tag in soup.find_all("a", href=True):
            href = a_tag["href"].strip()
            
            # 跳过特殊链接
            if href.startswith(("javascript:", "mailto:", "tel:", "#")):
                continue
            
            # 转换为绝对 URL
            if base_url:
                href = urljoin(base_url, href)
            
            # 验证 URL 格式
            if href.startswith(("http://", "https://")):
                links.append(href)
        
        return list(set(links))  # 去重
    
    def _extract_images(self, soup: BeautifulSoup, base_url: str) -> List[str]:
        """提取所有图片 URL"""
        images = []
        
        # 从 <img> 标签提取
        for img in soup.find_all("img"):
            src = img.get("src") or img.get("data-src") or img.get("data-lazy-src")
            if src:
                src = src.strip()
                if base_url:
                    src = urljoin(base_url, src)
                if src.startswith(("http://", "https://")):
                    images.append(src)
        
        # 从 CSS background-image 提取
        for tag in soup.find_all(style=True):
            style = tag["style"]
            urls = re.findall(r'url\(["\']?([^"\'()]+)["\']?\)', style)
            for url in urls:
                if base_url:
                    url = urljoin(base_url, url)
                if url.startswith(("http://", "https://")):
                    images.append(url)
        
        return list(set(images))  # 去重
    
    def _extract_scripts(self, soup: BeautifulSoup, base_url: str) -> List[str]:
        """提取所有脚本 URL"""
        scripts = []
        for script in soup.find_all("script", src=True):
            src = script["src"].strip()
            if base_url:
                src = urljoin(base_url, src)
            if src.startswith(("http://", "https://")):
                scripts.append(src)
        return list(set(scripts))
    
    def _extract_styles(self, soup: BeautifulSoup, base_url: str) -> List[str]:
        """提取所有样式表 URL"""
        styles = []
        for link in soup.find_all("link", rel="stylesheet"):
            href = link.get("href")
            if href:
                href = href.strip()
                if base_url:
                    href = urljoin(base_url, href)
                if href.startswith(("http://", "https://")):
                    styles.append(href)
        return list(set(styles))
    
    def _extract_headings(self, soup: BeautifulSoup) -> Dict[str, List[str]]:
        """提取所有标题"""
        headings = {}
        for level in range(1, 7):
            tag_name = f"h{level}"
            headings[tag_name] = []
            for heading in soup.find_all(tag_name):
                text = heading.get_text(strip=True)
                if text:
                    headings[tag_name].append(text)
        return headings
    
    def is_valid_html(self, html: str) -> bool:
        """
        检查是否为有效的 HTML
        
        Args:
            html: HTML 字符串
            
        Returns:
            是否有效
        """
        if not html:
            return False
        
        # 简单检查是否包含 HTML 标签
        html_pattern = re.compile(r'<\s*html|<\s*body|<\s*head|<\s*div|<\s*p\s', re.IGNORECASE)
        return bool(html_pattern.search(html))

