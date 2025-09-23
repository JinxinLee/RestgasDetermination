import os
import re

def generate_include_guard(file_path):
    # 基于文件路径生成唯一宏名称
    base = os.path.relpath(file_path).replace(os.sep, '_')
    guard = re.sub(r'\W+', '_', base).upper()
    return guard

def process_header(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    modified = False
    for i, line in enumerate(lines):
        if '#pragma once' in line:
            guard = generate_include_guard(file_path)
            lines[i] = f'#ifndef {guard}\n#define {guard}\n'
            lines.append(f'#endif // {guard}\n')
            modified = True
            break

    if modified:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(lines)
        print(f'✔️ Updated: {file_path}')

def walk_and_process(root_dir):
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith(('.h', '.hpp')):
                filepath = os.path.join(dirpath, filename)
                process_header(filepath)

# 💡 将下面路径修改为你的代码根目录
your_project_path = '/u/jili/workspace/oct19/tracking/'
walk_and_process(your_project_path)

