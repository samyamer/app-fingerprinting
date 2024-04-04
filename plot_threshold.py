import matplotlib.pyplot as plt

# Access times data
access_times = [351, 351, 360, 359, 358, 361, 351, 351, 351, 351, 359, 358, 361, 359, 504,
                352, 350, 351, 360, 359, 358, 358, 351, 351, 351, 351, 360, 359, 358, 362,
                351, 351, 351, 504, 359, 360, 359, 359, 351, 350, 351, 351, 360, 359, 359,
                358, 352, 351, 503]

# Generate x values (page indices starting from 2 and increasing by 2)
pages = range(2, 2 + 2 * len(access_times), 2)

# Plotting the bar chart
plt.figure(figsize=(15, 6))
bars = plt.bar(pages, access_times, color='skyblue')

# Color bars with values greater than 500 in red
for i in range(len(access_times)):
    if access_times[i] > 500:
        bars[i].set_color('red')

plt.xlabel('Page Index')
plt.ylabel('Access Time (Cycles)')
plt.title('Access Times for Each Page')

# Adjust x-axis ticks to match page indices
plt.xticks(pages)

# Adjust font size of tick labels
plt.tick_params(axis='both', which='major', labelsize=12)

plt.grid(True)

# Save the plot as a PNG file
plt.savefig('access_times_plot.png')

# Show the plot
plt.show()
