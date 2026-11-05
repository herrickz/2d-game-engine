
import argparse

from pathlib import Path
from PIL import Image

argument_parser = argparse.ArgumentParser("image_convert")

argument_parser.add_argument("image_path", help="File path to image")
argument_parser.add_argument("output_path", help="Output image path")

if __name__ == "__main__":

    args = argument_parser.parse_args()

    full_image_path = Path(args.image_path)

    image = Image.open(full_image_path.absolute())

    image.save(args.output_path, "PPM")

