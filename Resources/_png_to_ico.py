from PIL import Image
import argparse
import sys

def validate_image(img):
    """验证原始图片是否符合ICO转换要求"""
    # ICO格式最大支持256x256
    if img.width > 256 or img.height > 256:
        raise ValueError("ICO格式最大支持256x256像素，当前尺寸：{}x{}".format(img.width, img.height))
    
    # 必须为正方形
    if img.width != img.height:
        raise ValueError("必须使用正方形图片，当前尺寸：{}x{}".format(img.width, img.height))
    
    # 推荐尺寸检查
    recommended_sizes = [16, 24, 32, 48, 64, 128, 256]
    if img.width not in recommended_sizes:
        print("警告: 非推荐尺寸 {}x{}，建议使用以下尺寸之一：{}".format(
            img.width, img.height, recommended_sizes))

def png_to_ico(input_path, output_path):
    """
    原尺寸PNG转ICO工具
    生成与原始图片相同尺寸的ICO文件
    """
    try:
        with Image.open(input_path) as img:
            # 验证图片有效性
            # validate_image(img)
            
            # 转换为RGBA模式
            if img.mode != 'RGBA':
                img = img.convert('RGBA')
            
            # 直接保存为ICO格式
            img.save(
                output_path,
                format='ICO',
                bitmap_format='bmp',
                quality=100
            )
            
        print(f"成功生成原尺寸ICO文件: {output_path}")

    except Exception as e:
        print(f"转换失败: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='原尺寸PNG转ICO工具')
    parser.add_argument('input', help='输入的PNG文件路径')
    parser.add_argument('output', help='输出的ICO文件路径')
    
    args = parser.parse_args()
    
    # 执行转换
    png_to_ico(args.input, args.output)