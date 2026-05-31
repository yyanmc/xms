import os

base = r"D:\Code\cpp\C++微服务架构及安全云盘项目\xms0.1.1"
out_bin = base + "\\bin\\x64\\"

for proj in ['config_client','config_server','register_client','register_server']:
    path = os.path.join(proj, proj + '.vcxproj')
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    marker = "'$(Configuration)|$(Platform)'=='Release|x64'"
    tag = f'<PropertyGroup Condition="{marker}">'

    if tag in content and '<OutDir>' not in content[content.find(tag):content.find(tag)+200]:
        new_tag = tag + '\n    <OutDir>' + out_bin + '</OutDir>'
        content = content.replace(tag, new_tag)
        with open(path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f'Added OutDir to {path}')
    else:
        print(f'Skipped {path} (already has OutDir or no Release|x64)')

print('Done!')
