from PIL import Image
import os

def rescale_images_to_same_height(input_folder, output_folder, target_height):
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for filename in os.listdir(input_folder):
        if filename.endswith((".png", ".jpg", ".jpeg")):
            image_path = os.path.join(input_folder, filename)
            output_path = os.path.join(output_folder, filename)

            # Open the image using PIL
            image = Image.open(image_path)

            # Calculate the new width based on the target height and the aspect ratio
            width_percent = (target_height / float(image.size[1]))
            new_width = int((float(image.size[0]) * float(width_percent)))

            # Resize the image while maintaining the aspect ratio
            resized_image = image.resize((new_width, target_height), Image.LANCZOS)

            # Save the resized image to the output folder
            resized_image.save(output_path)

if __name__ == "__main__":
    input_folder = "."  # Replace with the path to the folder containing the images
    output_folder = "scaled/"  # Replace with the path to the output folder
    target_height = 300  # Replace with the desired target height in pixels

    rescale_images_to_same_height(input_folder, output_folder, target_height)
