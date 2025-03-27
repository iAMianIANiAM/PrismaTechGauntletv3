#!/usr/bin/env python3
"""
File Existence Verification Script

This script scans markdown files in the working/ directory to find file paths
and verifies that the referenced files actually exist in the project.
"""

import os
import re
import sys
import glob
import argparse
from pathlib import Path

# Colors for terminal output
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def find_file_references(md_file):
    """Extract file references from markdown file"""
    with open(md_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Patterns to look for
    patterns = [
        r'`([^`]*\.[ch]pp?)`',  # C/C++ files in backticks
        r'`([^`]*\.py)`',       # Python files in backticks
        r'`([^`]*\.md)`',       # Markdown files in backticks
        r'`([^`]*\.ini)`',      # INI files in backticks
        r'`(/[^`]*)`',          # Absolute paths in backticks
        r'`(src/[^`]*)`',       # src paths in backticks
        r'`(examples/[^`]*)`',  # examples paths in backticks
        r'`(utils/[^`]*)`',     # utils paths in backticks
        r'`(working/[^`]*)`',   # working paths in backticks
        r'`(include/[^`]*)`',   # include paths in backticks
        r'`(lib/[^`]*)`',       # lib paths in backticks
        r'`(test/[^`]*)`',      # test paths in backticks
    ]
    
    references = []
    for pattern in patterns:
        matches = re.findall(pattern, content)
        for match in matches:
            # Skip if it's clearly not a file path
            if not any(char in match for char in ['.', '/', '\\']):
                continue
            # Skip if it's a URL
            if match.startswith('http'):
                continue
            references.append(match)
    
    return references

def verify_file_exists(file_path, project_root):
    """Check if a file exists at the given path"""
    # Handle both absolute paths and paths relative to project root
    if file_path.startswith('/'):
        full_path = os.path.join(project_root, file_path[1:])
    else:
        full_path = os.path.join(project_root, file_path)
    
    # Check if the file exists
    return os.path.exists(full_path)

def is_planned_reference(md_file, file_path):
    """Check if a file reference is marked as planned in the document"""
    with open(md_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Look for patterns like "**Status**: [PLANNED]" near the file path
    context_pattern = r'{}.*?(?:\[PLANNED\]|\*\*Status\*\*:\s*\[PLANNED\])'.format(re.escape(file_path))
    return bool(re.search(context_pattern, content, re.DOTALL | re.IGNORECASE))

def main():
    parser = argparse.ArgumentParser(description='Verify file references in documentation')
    parser.add_argument('--report', action='store_true', help='Generate summary report only')
    parser.add_argument('--verbose', action='store_true', help='Show detailed information')
    parser.add_argument('--all', action='store_true', help='Check all markdown files, not just working/')
    args = parser.parse_args()
    
    # Get project root directory
    project_root = Path(__file__).parent.parent.absolute()
    
    # Find markdown files to scan
    if args.all:
        md_files = glob.glob(str(project_root / '**' / '*.md'), recursive=True)
    else:
        md_files = glob.glob(str(project_root / 'working' / '*.md'))
    
    # Track overall statistics
    total_references = 0
    missing_references = 0
    planned_references = 0
    
    # Track missing files
    missing_files = []
    
    for md_file in md_files:
        rel_path = os.path.relpath(md_file, project_root)
        if args.verbose:
            print(f"{Colors.HEADER}Checking {rel_path}{Colors.ENDC}")
        
        file_references = find_file_references(md_file)
        file_specific_missing = 0
        
        for file_path in file_references:
            total_references += 1
            exists = verify_file_exists(file_path, project_root)
            
            if not exists:
                # Check if the file is marked as planned
                is_planned = is_planned_reference(md_file, file_path)
                
                if is_planned:
                    planned_references += 1
                    if args.verbose:
                        print(f"  {Colors.WARNING}[PLANNED] {file_path}{Colors.ENDC}")
                else:
                    missing_references += 1
                    file_specific_missing += 1
                    if not args.report or args.verbose:
                        print(f"  {Colors.FAIL}[MISSING] {file_path}{Colors.ENDC}")
                    missing_files.append((rel_path, file_path))
        
        if args.verbose and file_specific_missing == 0:
            print(f"  {Colors.OKGREEN}All references exist or are marked as planned{Colors.ENDC}")
    
    # Print summary
    print(f"{Colors.BOLD}File Reference Verification Summary:{Colors.ENDC}")
    print(f"  Documents scanned: {len(md_files)}")
    print(f"  Total file references: {total_references}")
    print(f"  Files marked as [PLANNED]: {planned_references}")
    print(f"  Missing files (not planned): {missing_references}")
    
    if missing_references > 0:
        print(f"\n{Colors.FAIL}Missing files:{Colors.ENDC}")
        for doc, missing in missing_files:
            print(f"  {Colors.FAIL}{doc}: {missing}{Colors.ENDC}")
        return 1
    else:
        print(f"\n{Colors.OKGREEN}All file references exist or are properly marked as [PLANNED]{Colors.ENDC}")
        return 0

if __name__ == '__main__':
    sys.exit(main()) 