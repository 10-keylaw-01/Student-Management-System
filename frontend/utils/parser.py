"""Utilities for parsing C++ console output."""

import re
from typing import List, Dict, Tuple, Optional


class OutputParser:
    """Parse C++ console output into structured data."""
    
    @staticmethod
    def parse_table(output: str) -> Optional[List[Dict]]:
        """Parse table output from C++ into list of dicts."""
        lines = output.strip().split('\n')
        if len(lines) < 2:
            return None
        
        # Remove decorative lines
        lines = [l for l in lines if not re.match(r'^[\s\-\+\|]*$', l)]
        if not lines:
            return None
        
        # Extract headers
        header_line = lines[0]
        headers = [h.strip() for h in re.split(r'\|', header_line) if h.strip()]
        
        if not headers:
            return None
        
        # Extract rows
        rows = []
        for line in lines[1:]:
            cells = [c.strip() for c in re.split(r'\|', line) if c.strip()]
            if len(cells) == len(headers):
                rows.append(dict(zip(headers, cells)))
        
        return rows if rows else None
    
    @staticmethod
    def parse_menu(output: str) -> Optional[List[Tuple[str, str]]]:
        """Parse menu options from output."""
        lines = output.strip().split('\n')
        options = []
        
        for line in lines:
            # Match pattern like "1. Option text"
            match = re.match(r'\s*(\d+)\.\s+(.+)', line)
            if match:
                options.append((match.group(1), match.group(2)))
        
        return options if options else None
    
    @staticmethod
    def extract_error(output: str) -> Optional[str]:
        """Extract error message from output."""
        if '[!]' in output or '[ERROR]' in output or 'error' in output.lower():
            return output.strip()
        return None
    
    @staticmethod
    def extract_success(output: str) -> Optional[str]:
        """Extract success message from output."""
        if '✓' in output or '[✓]' in output or '[SUCCESS]' in output:
            return output.strip()
        return None
    
    @staticmethod
    def extract_info(output: str) -> Optional[str]:
        """Extract info message from output."""
        lines = output.strip().split('\n')
        for line in lines:
            if line.startswith('[') or line.startswith('├') or line.startswith('│'):
                return line.strip()
        return None if not output else output.strip()
