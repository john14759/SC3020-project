import tkinter as tk
from PIL import Image, ImageTk
import tkinter.font as tkFont
import requests

#Function to generate legend items on legend canvas
def create_legend_items(canvas, items, x_start, y_start, y_gap, line_width, font_size):
    bold_font = tkFont.Font(family="Helvetica", size=font_size, weight="bold")
    regular_font = tkFont.Font(family="Helvetica", size=font_size)

    y = y_start
    for item in items:
        text = item["text"]
        title, description = text.split(":", 1)

        # Create bold title
        canvas.create_text(x_start, y, text=title + ":", font=bold_font, anchor="nw")

        # Calculate width of the bold title to position the description correctly
        title_width = bold_font.measure(title + ":")
        description_x = x_start + title_width

        # Create regular description
        canvas.create_text(description_x, y, text=description, font=regular_font, anchor="nw")

        y += y_gap

def create_legend(left_frame, legend_items, create_legend_flag, legend_canvas):

    if not create_legend_flag:
        # Check if legend_canvas already exists, if not create it
        if legend_canvas is None:
            legend_canvas_width = 400
            legend_canvas_height = len(legend_items) * 40 + 10
            font_size = 10
            legend_canvas = tk.Canvas(left_frame, width=legend_canvas_width, height=legend_canvas_height)
            legend_canvas.pack(pady=10)

        # Create legend items
        create_legend_items(legend_canvas, legend_items, 20, 10, 40, legend_canvas_width - 30, font_size)
        create_legend_flag = True

# Function to create scrollable canvas
def create_scrollable_canvas(parent, side=tk.LEFT, padx=10, pady=10, min_width=400):
    # Create a frame to contain the canvas and scrollbar
    container = tk.Frame(parent)
    container.pack(side=side, fill="both", expand=True, padx=padx, pady=pady)

    # Create the canvas inside the container
    canvas = tk.Canvas(container, width=min_width)
    canvas.pack(side=tk.LEFT, fill="both", expand=True)

    # Create the scrollbar inside the container
    scrollbar = tk.Scrollbar(container, orient="vertical", command=canvas.yview)
    scrollbar.pack(side="right", fill="y")

    # Configure the canvas
    canvas.configure(yscrollcommand=scrollbar.set)
    frame = tk.Frame(canvas, width=min_width)
    canvas_frame = canvas.create_window((min_width/2, 0), window=frame, anchor='nw')

    frame.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

    def on_canvas_configure(event):
        canvas.itemconfig(canvas_frame, width=event.width)
        canvas.move(canvas_frame, event.width/2 - canvas.coords(canvas_frame)[0], 0)
        canvas.config(scrollregion=canvas.bbox("all"))

    # Function to handle mouse scroll
    def _on_mousewheel(event):
        canvas.yview_scroll(int(-1*(event.delta/120)), "units")

    # Binding mouse scroll events to this specific canvas
    canvas.bind("<MouseWheel>", _on_mousewheel)  # for Windows and MacOS

    canvas.bind("<Enter>", lambda _: canvas.bind_all("<MouseWheel>", _on_mousewheel))  # Re-bind when entering canvas
    canvas.bind("<Leave>", lambda _: canvas.unbind_all("<MouseWheel>"))  # Unbind when leaving canvas
    
    return canvas, frame

def resize_image(image_path, max_size):
    image = Image.open(image_path)
    original_size = image.size

    ratio = min(max_size[0] / original_size[0], max_size[1] / original_size[1])
    new_size = tuple([int(x * ratio) for x in original_size])
    
    resized_image = image.resize(new_size, Image.Resampling.LANCZOS)
    return resized_image

def open_fullsize_image():
    new_window = tk.Toplevel()
    new_window.title("Full-Size QEP Image")

    # Load the full-size image
    fullsize_image = Image.open("qep_tree.png")
    photo_image = ImageTk.PhotoImage(fullsize_image)

    # Get screen width and height
    screen_width = new_window.winfo_screenwidth()
    screen_height = new_window.winfo_screenheight()

    # Set max dimensions for the window
    max_window_width = min(fullsize_image.width, screen_width - 100)  # 100 pixels less than screen width
    max_window_height = min(fullsize_image.height, screen_height - 100)  # 100 pixels less than screen height

    # Set the geometry of the new window
    new_window.geometry(f"{max_window_width}x{max_window_height}")

    # Create canvas and scrollbars
    canvas = tk.Canvas(new_window)
    scrollbar_y = tk.Scrollbar(new_window, orient="vertical", command=canvas.yview)
    scrollbar_x = tk.Scrollbar(new_window, orient="horizontal", command=canvas.xview)
    canvas.configure(yscrollcommand=scrollbar_y.set, xscrollcommand=scrollbar_x.set)

    scrollbar_y.pack(side="right", fill="y")
    scrollbar_x.pack(side="bottom", fill="x")
    canvas.pack(side="left", fill="both", expand=True)

    # Add image to canvas
    canvas.create_image(0, 0, image=photo_image, anchor="nw")

    # Configure scroll region
    canvas.config(scrollregion=canvas.bbox("all"))
    canvas.image = photo_image  # Keep a reference

def view_statement_details(window, detail):
    details_window = tk.Toplevel(window)
    details_window.title("Details")
    match detail["Node Type"]:
        case "Seq Scan": seq_scan_visualisation(details_window, detail)
        case "Hash": hash_visualisation(details_window, detail)
        case "Hash Join": hash_join_visualisation(details_window, detail)
        case "Aggregate": aggregate_visualisation(details_window, detail)
        case "Nested Loop": nested_loop_visualisation(details_window, detail)
        case _ : tk.Label(details_window, text=f"No visualisation available for this operation", font=("Helvetica", 20)).pack(padx=10, pady=10)

def resize_image_aspect_ratio(image, max_size):
    original_size = image.size
    ratio = min(max_size[0] / original_size[0], max_size[1] / original_size[1])
    new_size = tuple([int(x * ratio) for x in original_size])
    return image.resize(new_size, Image.Resampling.LANCZOS)

def get_image(url):
    response = requests.get(url, stream=True)
    img = Image.open(response.raw)
    return img

def seq_scan_visualisation(details_window, detail):
    original_image = get_image("https://postgrespro.com/media/2022/03/31/seqscan1-en.png")
    # Resize the image
    resized_im = resize_image_aspect_ratio(original_image, (1000, 1000))
    seq_scan_im = ImageTk.PhotoImage(resized_im)
    label = tk.Label(details_window)
    label.config(image=seq_scan_im)
    label.image= seq_scan_im
    label.pack(padx=10, pady=10)
    relation_name = detail["Relation Name"]
    blks_hit = str(detail["Shared Hit Blocks"])
    num_rows = str(detail["Actual Rows"])
    tk.Label(details_window, text=f"Number of {relation_name} data block(s) read into buffer: {blks_hit}", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"Number of tuple matches: {num_rows}", font=("Helvetica", 15)).pack(pady=5)

def hash_visualisation(details_window, detail):
    original_image = get_image("https://postgrespro.com/media/2019/05/23/i3.png")
    # Resize the image
    resized_im = resize_image_aspect_ratio(original_image, (1000, 1000))
    hash_im = ImageTk.PhotoImage(resized_im)
    label = tk.Label(details_window)
    label.config(image=hash_im)
    label.image= hash_im
    label.pack(padx=10, pady=10)
    blks_hit = str(detail["Shared Hit Blocks"])
    num_buckets = str(detail["Hash Buckets"])
    num_rows = str(detail["Actual Rows"])
    tk.Label(details_window, text=f"Buckets available: {num_buckets}", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"{num_rows} tuple(s) from {blks_hit} data block(s) hashed into buckets.", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"A hash table with hash attribute to bucket mapping is stored in the buffer.", font=("Helvetica", 15)).pack(pady=5)

def hash_join_visualisation(details_window, detail):
    original_image = get_image("https://postgrespro.com/media/2022/08/11/hash1-en.png")
    # Resize the image
    resized_im = resize_image_aspect_ratio(original_image, (1000, 1000))
    hash_join_im = ImageTk.PhotoImage(resized_im)
    label = tk.Label(details_window)
    label.config(image=hash_join_im)
    label.image = hash_join_im
    label.pack(padx=10, pady=10)
    inner_set = detail["inner_set"]
    outer_set = detail["outer_set"]
    outer_set_rows = detail["Actual Rows"]
    tk.Label(details_window, text=f"Buckets of {inner_set} data blocks as the inner set.", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"{outer_set_rows} tuples in {outer_set} data blocks previously read into the buffer as the outer set.", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"For each tuple in the outer set, the hash table is probed for the matching bucket. Then the outer set tuple joins with each tuple in the bucket.", font=("Helvetica", 15)).pack(pady=5)

def aggregate_visualisation(details_window, detail):
    original_image = get_image("https://postgrespro.com/media/2022/03/31/seqscan1-en.png")
    # Resize the image
    resized_im = resize_image_aspect_ratio(original_image, (1000, 1000))
    aggregate_im = ImageTk.PhotoImage(resized_im)
    label = tk.Label(details_window)
    label.config(image=aggregate_im)
    label.image = aggregate_im
    label.pack(padx=10, pady=10)
    num_output_rows = detail["Actual Rows"]
    num_rows_removed = detail["Rows Removed by Filter"]
    num_input_rows = num_output_rows + num_rows_removed
    tk.Label(details_window, text=f"Number of input tuples: {num_input_rows}", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"Number of output tuples: {num_output_rows}", font=("Helvetica", 15)).pack(pady=5)

def nested_loop_visualisation(details_window, detail):
    original_image = get_image("http://www.interdb.jp/pg/img/fig-3-16.png")
    # Resize the image
    resized_im = resize_image_aspect_ratio(original_image, (1000, 1000))
    nested_loop_im = ImageTk.PhotoImage(resized_im)
    label = tk.Label(details_window)
    label.config(image=nested_loop_im)
    label.image = nested_loop_im
    label.pack(padx=10, pady=10)
    num_output_rows = detail["Actual Rows"]
    num_inner_rows = detail["inner_rows"]
    num_other_rows = detail["outer_rows"]
    num_loops = detail["Actual Loops"]
    tk.Label(details_window, text=f"{num_inner_rows} inner tuple(s) join on {num_other_rows} outer tuple(s), output {num_output_rows} tuples.", font=("Helvetica", 15)).pack(pady=5)
    tk.Label(details_window, text=f"{num_loops} loop(s) required.", font=("Helvetica", 15)).pack(pady=5)
