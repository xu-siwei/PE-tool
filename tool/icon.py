from PIL import Image
import sys
import os
import shutil

# 需要生成的多尺寸图标
SIZES = [16, 32, 64, 128, 256]

def image_to_c_array(img: Image.Image, var_name: str) -> str:
    """把 RGBA 图像转换为 C 数组字符串"""
    rgba = img.convert("RGBA").tobytes()
    data = ",".join(str(b) for b in rgba)
    return f"static const unsigned char {var_name}[] = {{ {data} }};\n"

def generate_icons_header(input_png: str, output_header: str):
    img = Image.open(input_png)

    with open(output_header, "w", encoding="utf-8") as f:
        f.write("// 自动生成的 icons.h\n")
        f.write("#pragma once\n")
        f.write("#include \"sokol_app.h\"\n\n")

        var_names = []
        # 生成每个尺寸的 RGBA 数组
        for size in SIZES:
            resized = img.resize((size, size), Image.LANCZOS)
            var_name = f"icon_{size}x{size}_rgba"
            var_names.append((var_name, size, size))
            f.write(image_to_c_array(resized, var_name))

        # 生成 sapp_icon_desc
        f.write("\nstatic const sapp_icon_desc app_icon = {\n")
        f.write("    .sokol_default = false,\n")
        f.write("    .images = {\n")

        # 已生成的尺寸
        for (var_name, w, h) in var_names:
            f.write("        {\n")
            f.write(f"            .width = {w},\n")
            f.write(f"            .height = {h},\n")
            f.write(f"            .pixels = {var_name}\n")
            f.write("        },\n")

        # ⚠️ 补齐到 8 个 entry，避免 sokol 访问未初始化数据
        for _ in range(len(var_names), 8):
            f.write("        { .width = 0, .height = 0, .pixels = 0 },\n")

        f.write("    }\n};\n")

        # 加一个静态检查（编译时）保证数组大小正确
        f.write("\n_Static_assert(sizeof(app_icon.images)/sizeof(app_icon.images[0]) == SAPP_MAX_ICONIMAGES, "
                "\"icon desc must match sokol_app.h definition\");\n")

    print(f"[OK] 已生成 {output_header}")

if __name__ == "__main__":
    generate_icons_header("icon.png", "icons.h")
    shutil.move("icons.h", "../src")