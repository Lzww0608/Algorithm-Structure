#!/usr/bin/env python3
"""
简易网络爬虫 - 入口程序

使用示例:
    python main.py --url https://example.com --max-pages 10
    python main.py --urls urls.txt --max-depth 2
"""

import argparse
import sys
import signal
from typing import List

from config import CrawlerConfig
from crawler import WebCrawler


def parse_args():
    """解析命令行参数"""
    parser = argparse.ArgumentParser(
        description="简易网络爬虫",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s --url https://example.com
  %(prog)s --url https://example.com --max-pages 50 --max-depth 3
  %(prog)s --urls seed_urls.txt --workers 10
        """
    )
    
    # URL 参数
    url_group = parser.add_mutually_exclusive_group(required=True)
    url_group.add_argument(
        "--url", "-u",
        type=str,
        help="要爬取的单个 URL"
    )
    url_group.add_argument(
        "--urls", "-U",
        type=str,
        help="包含种子 URL 的文件（每行一个 URL）"
    )
    
    # 爬取参数
    parser.add_argument(
        "--max-pages", "-p",
        type=int,
        default=100,
        help="最大爬取页面数（默认: 100）"
    )
    parser.add_argument(
        "--max-depth", "-d",
        type=int,
        default=3,
        help="最大爬取深度（默认: 3）"
    )
    parser.add_argument(
        "--workers", "-w",
        type=int,
        default=5,
        help="并发工作线程数（默认: 5）"
    )
    parser.add_argument(
        "--delay",
        type=float,
        default=1.0,
        help="同一域名请求间隔秒数（默认: 1.0）"
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=10,
        help="请求超时秒数（默认: 10）"
    )
    
    # 输出参数
    parser.add_argument(
        "--output", "-o",
        type=str,
        default="./crawled_data",
        help="输出目录（默认: ./crawled_data）"
    )
    
    # 过滤参数
    parser.add_argument(
        "--allowed-domains",
        type=str,
        nargs="*",
        help="只爬取指定域名（留空则爬取所有）"
    )
    parser.add_argument(
        "--blocked-domains",
        type=str,
        nargs="*",
        help="不爬取的域名黑名单"
    )
    
    # 功能开关
    parser.add_argument(
        "--no-images",
        action="store_true",
        help="不下载图片"
    )
    parser.add_argument(
        "--no-monitor",
        action="store_true",
        help="禁用网页监控"
    )
    
    # 用户代理
    parser.add_argument(
        "--user-agent",
        type=str,
        default="SimpleCrawler/1.0 (Educational Purpose)",
        help="自定义 User-Agent"
    )
    
    return parser.parse_args()


def load_urls_from_file(filepath: str) -> List[str]:
    """从文件加载 URL 列表"""
    urls = []
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    urls.append(line)
    except FileNotFoundError:
        print(f"错误: 文件不存在 - {filepath}")
        sys.exit(1)
    except Exception as e:
        print(f"错误: 读取文件失败 - {e}")
        sys.exit(1)
    
    return urls


def main():
    """主函数"""
    args = parse_args()
    
    # 创建配置
    config = CrawlerConfig(
        max_workers=args.workers,
        max_depth=args.max_depth,
        max_pages=args.max_pages,
        request_timeout=args.timeout,
        request_delay=args.delay,
        user_agent=args.user_agent,
        storage_dir=args.output,
        content_dir=f"{args.output}/content",
        image_dir=f"{args.output}/images",
        enable_image_download=not args.no_images,
        enable_web_monitor=not args.no_monitor
    )
    
    # 处理域名限制
    if args.allowed_domains:
        config.allowed_domains = set(args.allowed_domains)
    
    if args.blocked_domains:
        config.blocked_domains.update(args.blocked_domains)
    
    # 创建爬虫
    crawler = WebCrawler(config)
    
    # 设置信号处理（Ctrl+C 优雅退出）
    def signal_handler(sig, frame):
        print("\n收到中断信号，正在停止爬虫...")
        crawler.stop()
    
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # 获取种子 URL
    if args.url:
        seed_urls = [args.url]
    else:
        seed_urls = load_urls_from_file(args.urls)
    
    if not seed_urls:
        print("错误: 没有有效的种子 URL")
        sys.exit(1)
    
    print(f"种子 URL 数量: {len(seed_urls)}")
    
    # 添加种子 URL
    crawler.add_seed_urls(seed_urls)
    
    # 开始爬取
    try:
        stats = crawler.crawl(max_pages=args.max_pages)
        
        # 返回状态码
        if stats.pages_crawled > 0:
            sys.exit(0)
        else:
            sys.exit(1)
    
    except KeyboardInterrupt:
        print("\n爬取被中断")
        sys.exit(130)
    
    except Exception as e:
        print(f"错误: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()

