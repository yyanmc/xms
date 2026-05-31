import os, re

base = r"D:\Code\cpp\C++微服务架构及安全云盘项目\xms0.1.1"
out_bin = base + "\\bin\\x64\\"
out_lib = base + "\\lib\\x64\\"

for root, dirs, files in os.walk("."):
    for f in files:
        if not f.endswith(".vcxproj"):
            continue
        path = os.path.join(root, f)
        with open(path, "r", encoding="utf-8") as fh:
            content = fh.read()

        modified = False

        # Fix OutDir for Release|x64 - replace relative path with absolute
        new_content = content.replace(
            "<OutDir>..\\bin\\x64\\</OutDir>",
            f"<OutDir>{out_bin}</OutDir>"
        )
        new_content = new_content.replace(
            "<OutDir>..\\bin\\x64</OutDir>",
            f"<OutDir>{out_bin}</OutDir>"
        )
        if new_content != content:
            modified = True
            content = new_content

        # Fix Lib OutputFile
        new_content = content.replace(
            "<OutputFile>..\\lib\\x64\\",
            f"<OutputFile>{out_lib}"
        )
        if new_content != content:
            modified = True
            content = new_content

        # Fix ImportLibrary
        new_content = content.replace(
            "<ImportLibrary>..\\lib\\x64\\",
            f"<ImportLibrary>{out_lib}"
        )
        if new_content != content:
            modified = True
            content = new_content

        if modified:
            with open(path, "w", encoding="utf-8") as fh:
                fh.write(content)
            print(f"Fixed: {path}")

print("Done!")
