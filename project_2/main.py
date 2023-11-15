import tkinter as tk
from PIL import Image, ImageTk
from test import connection_to_db, get_qep_image, get_qep_statements

# Create the main window
window = tk.Tk()
window.title("SQL Query Executor")

# Set the window size to cover the entire screen
# window.geometry(f"{window.winfo_screenwidth()}x{window.winfo_screenheight()}+0+0")

# Create a top canvas for title, entry field, and button
top_canvas = tk.Canvas(window)
top_canvas.pack(side=tk.TOP, padx=10, pady=10)

# Create a title label with a big font in the top canvas
title_label = tk.Label(top_canvas, text="Enter SQL Query", font=("Helvetica", 15))
title_label.pack()

# Create an entry field for the SQL query in the top canvas
sql_entry = tk.Entry(top_canvas, width=50, font=("Helvetica", 10))
sql_entry.pack(pady=10)

# Function to create scrollable canvas
def create_scrollable_canvas(parent, side=tk.LEFT, padx=10, pady=10, min_width=400):
    canvas = tk.Canvas(parent, width=min_width)
    scrollbar = tk.Scrollbar(parent, orient="vertical", command=canvas.yview)
    canvas.configure(yscrollcommand=scrollbar.set)
    canvas.pack(side=side, fill="both", expand=True, padx=padx, pady=pady)
    scrollbar.pack(side="right", fill="y")
    frame = tk.Frame(canvas, width=min_width)
    canvas_frame = canvas.create_window((min_width/2, 0), window=frame, anchor='center')
    frame.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

    def on_canvas_configure(event):
        canvas.itemconfig(canvas_frame, x=event.width/2)

    canvas.bind("<Configure>", on_canvas_configure)
    return canvas, frame

#Function to generate legend items on legend canvas
def create_legend_items(canvas, legend_items, start_x, start_y, line_height, text_width, font_size):
    bullet_diameter = 4
    bullet_radius = bullet_diameter // 2
    text_vertical_offset = font_size // 4
    for i, item in enumerate(legend_items):
        y_offset = start_y + i * line_height  # Adjust Y-coordinate to separate items
        if( i < 2):
            canvas.create_oval(start_x - bullet_radius, y_offset + text_vertical_offset - bullet_radius, start_x + bullet_radius, y_offset + text_vertical_offset + bullet_radius, fill="black")
            canvas.create_text(start_x + 10, y_offset + 10, text=item["text"], anchor="w", font=("Helvetica", 10), width=text_width)
        else:
            canvas.create_oval(start_x - bullet_radius, y_offset + 10 - bullet_radius, start_x + bullet_radius, y_offset + 10 + bullet_radius, fill="black")
            canvas.create_text(start_x + 10, y_offset + 10, text=item["text"], anchor="w", font=("Helvetica", 10), width=text_width)

# Function to execute the SQL query
def execute_sql_query():
    global legend_canvas  # Declare legend_canvas as a global variable
    query = sql_entry.get()

    try:
        connection, cursor = connection_to_db()
        cursor.execute(query)

        # Fetch the QEP image
        qep_digraph = get_qep_image(cursor, query)
        statements = get_qep_statements(cursor, query)
        cursor.close()
        connection.close()

        # Save the QEP digraph as a PNG file
        qep_digraph.format = 'png'
        qep_digraph.render(filename="qep_tree")

        # Open the QEP image and convert it to Tkinter PhotoImage
        qep_image = Image.open("qep_tree.png")

        # Resize the QEP image to your desired dimensions
        resized_qep_img = qep_image.resize((600, 600))  # Adjust the dimensions as needed
        resized_qep_img.save("resized_qep_tree.png")

        qep_image = ImageTk.PhotoImage(resized_qep_img)

        qep_label.config(image=qep_image)
        qep_label.image = qep_image
        qep_label.pack(side="top", fill="both", expand=True)

        # Update the statements in the right frame
        analysis_output_label.config(text='\n'.join(statements))
        analysis_output_label.pack(side="top", fill="both", expand=True)

        # Create a legend canvas in the left frame
        legend_canvas_width = 400
        legend_canvas_height = len(legend_items) * 40 + 10  # Adjust the height based on the number of items
        font_size = 10
        legend_canvas = tk.Canvas(left_frame, width=legend_canvas_width, height=legend_canvas_height)
        legend_canvas.pack(pady=10)
        
        #Create legend items
        create_legend_items(legend_canvas, legend_items, 20, 10, 40, legend_canvas_width - 30, font_size)

        # Create legend items with font
        # for i, item in enumerate(legend_items):
        #     y_offset = i * 40  # Adjust Y-coordinate to separate items
        #     legend_canvas.create_oval(5, 5 + y_offset, 15, 15 + y_offset, fill="black")
        #     legend_canvas.create_text(20, 10 + y_offset, text=item["text"], anchor="w", font=("Helvetica", 10))

    except Exception as e:
        result_label.config(text=f"Error: {str(e)}")

# Create a button to execute the SQL query in the top canvas
execute_button = tk.Button(top_canvas, text="Execute Query", command=execute_sql_query, font=("Helvetica", 10))
execute_button.pack(pady=10)

# Create a left canvas for QEP image and legend items
# left_canvas = tk.Canvas(window)
# left_canvas.pack(side=tk.LEFT, padx=10, pady=10)

# Create a right canvas for QEP analysis output
# right_canvas = tk.Canvas(window)
# right_canvas.pack(side=tk.RIGHT, padx=10, pady=10, fill=tk.Y)

# Create scrollable left and right canvases
left_canvas, left_frame = create_scrollable_canvas(window, side=tk.LEFT, min_width=400)
right_canvas, right_frame = create_scrollable_canvas(window, side=tk.RIGHT, min_width=400)

# Create a label to display the QEP analysis output in the right canvas
analysis_output_label = tk.Label(right_frame, text="", font=("Helvetica", 12), justify=tk.LEFT)
analysis_output_label.pack()

# Legend items
legend_items = [

    {"text": "Start-up cost is the estimated Postgres computational units to start up a node to start process the query."},
    {"text": "Total cost is the estimated Postgres computational units to finish process the query and return results"},
    {"text": "Shared Hit Blocks: number of shared blocks read into cache"},
    {"text": "Local Hit Blocks: number of local blocks read into cache"},
]

# Create a label to display the QEP image in the left canvas
qep_label = tk.Label(left_frame, font=("Helvetica", 12))
qep_label.pack()

# Create a label to display the result in the left canvas
result_label = tk.Label(left_frame, text="", font=("Helvetica", 12))
result_label.pack(pady=10)

# Start the mainloop
window.mainloop()
