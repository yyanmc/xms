import os
base = r"D:\Code\cpp\C++微服务架构及安全云盘项目\xms0.1.1"
out_bin = base + "\\bin\\x64\\"
out_lib = base + "\\lib\\x64\\"

for proj in ['config_client','config_server','register_client','register_server']:
    path = os.path.join(proj, proj + '.vcxproj')
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    modified = False
    if '..\\..\\bin\\x64\\' in content:
        content = content.replace('..\\..\\bin\\x64\\', out_bin)
        modified = True
    if '..\\..\\lib\\x64\\' in content:
        content = content.replace('..\\..\\lib\\x64\\', out_lib)
        modified = True
    if modified:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f'Fixed: {path}')
    else:
        print(f'OK: {path}')

print('Done!')
