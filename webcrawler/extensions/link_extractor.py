"""
Link Extractor - 链接提取器扩展

功能:
1. 从页面中提取所有链接
2. 分类链接（内部/外部）
3. 提供链接统计
"""

from typing import List, Dict, Set
from urllib.parse import urlparse, urljoin
from .base import Extension, ExtensionResult


class LinkExtractor(Extension):
    """
    链接提取器
    
    从 HTML 页面中提取链接
    """
    
    def __init__(self, base_domain: str = None):
        """
        初始化链接提取器
        
        Args:
            base_domain: 基础域名（用于区分内部/外部链接）
        """
        super().__init__("LinkExtractor")
        self._base_domain = base_domain
        self._total_links = 0
        self._internal_links = 0
        self._external_links = 0
        self._unique_domains: Set[str] = set()
    
    def process(self, url: str, content: str, parsed_data: dict) -> ExtensionResult:
        """
        提取页面链接
        
        Args:
            url: 页面 URL
            content: 页面内容
            parsed_data: 解析后的数据
            
        Returns:
            ExtensionResult 包含提取的链接
        """
        try:
            # 从 parsed_data 获取链接（如果解析器已经提取）
            links = parsed_data.get('links', [])
            
            if not links and content:
                # 如果没有预解析的链接，自行提取
                links = self._extract_links_from_html(content, url)
            
            # 分类链接
            current_domain = self._get_domain(url)
            internal_links = []
            external_links = []
            
            for link in links:
                link_domain = self._get_domain(link)
                self._unique_domains.add(link_domain)
                
                if self._is_internal_link(link_domain, current_domain):
                    internal_links.append(link)
                    self._internal_links += 1
                else:
                    external_links.append(link)
                    self._external_links += 1
            
            self._total_links += len(links)
            
            result_data = {
                "all_links": links,
                "internal_links": internal_links,
                "external_links": external_links,
                "total_count": len(links),
                "internal_count": len(internal_links),
                "external_count": len(external_links)
            }
            
            return ExtensionResult(
                extension_name=self.name,
                success=True,
                data=result_data
            )
        
        except Exception as e:
            return ExtensionResult(
                extension_name=self.name,
                success=False,
                error_message=str(e)
            )
    
    def _extract_links_from_html(self, html: str, base_url: str) -> List[str]:
        """从 HTML 提取链接"""
        from bs4 import BeautifulSoup
        
        links = []
        soup = BeautifulSoup(html, 'lxml')
        
        for a_tag in soup.find_all('a', href=True):
            href = a_tag['href'].strip()
            
            # 跳过特殊链接
            if href.startswith(('javascript:', 'mailto:', 'tel:', '#')):
                continue
            
            # 转换为绝对 URL
            absolute_url = urljoin(base_url, href)
            
            # 验证 URL
            if absolute_url.startswith(('http://', 'https://')):
                links.append(absolute_url)
        
        return list(set(links))
    
    def _get_domain(self, url: str) -> str:
        """提取域名"""
        try:
            parsed = urlparse(url)
            domain = parsed.netloc.lower()
            if domain.startswith('www.'):
                domain = domain[4:]
            return domain
        except Exception:
            return ""
    
    def _is_internal_link(self, link_domain: str, current_domain: str) -> bool:
        """判断是否为内部链接"""
        base = self._base_domain or current_domain
        
        if base.startswith('www.'):
            base = base[4:]
        
        return link_domain == base or link_domain.endswith('.' + base)
    
    def set_base_domain(self, domain: str):
        """设置基础域名"""
        self._base_domain = domain.lower()
        if self._base_domain.startswith('www.'):
            self._base_domain = self._base_domain[4:]
    
    def get_unique_domains(self) -> Set[str]:
        """获取所有发现的唯一域名"""
        return self._unique_domains.copy()
    
    def stats(self) -> dict:
        """获取统计信息"""
        base_stats = super().stats()
        base_stats.update({
            "total_links": self._total_links,
            "internal_links": self._internal_links,
            "external_links": self._external_links,
            "unique_domains": len(self._unique_domains)
        })
        return base_stats

