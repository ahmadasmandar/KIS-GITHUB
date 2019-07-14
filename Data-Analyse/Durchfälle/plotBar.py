"""
* This file written by Ahmad Asmandar 14-07-2019
* Reference  :https://matplotlib.org/3.1.0/api/_as_gen/matplotlib.pyplot.bar.html#matplotlib.pyplot.bar
"""
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
############# here we used 10 balls result###############

hall_1=[1,1,1,1,1,1,1,1,1,0]
hall_2=[1,1,1,1,1,1,1,1,1,0]
hall_3=[1,1,1,1,1,1,1,1,0,0]
#############################
photo_1=[1,1,1,1,1,1,1,1,0,0]
photo_2=[1,1,1,1,1,1,1,1,0,0]
photo_3=[1,1,1,1,1,1,1,1,0,0]
#############################
beide_2=[1,1,1,1,1,1,1,1,0,0]
######################################
hall_score, hall_std = (10*sum(hall_1), 10*sum(hall_2), 10*sum(hall_3), 10*sum(beide_2)), (3, 3, 2, 1)
photo_score, photo_std = (10*sum(photo_1), 10*sum(photo_2), 10*sum(photo_3), 10*sum(beide_2)), (3, 3, 2, 1)

ind = np.arange(len(hall_score))  # the x locations for the groups
width = 0.35  # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(ind - width/2, hall_score, width, yerr=hall_std,
                label='photo')
rects2 = ax.bar(ind + width/2, photo_score, width, yerr=photo_std,
                label='hall')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('Scores in %')
ax.set_title('Scores for all modes')
ax.set_xticks(ind)
ax.set_xticklabels(('1 U/s', '2 U/s', '3 U/s', 'Beide'))
ax.legend()


def autolabel(rects, xpos='center'):
    """
    Attach a text label above each bar in *rects*, displaying its height.

    *xpos* indicates which side to place the text w.r.t. the center of
    the bar. It can be one of the following {'center', 'right', 'left'}.
    """

    ha = {'center': 'center', 'right': 'left', 'left': 'right'}
    offset = {'center': 0, 'right': 1, 'left': -1}

    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(offset[xpos]*3, 3),  # use 3 points offset
                    textcoords="offset points",  # in both directions
                    ha=ha[xpos], va='bottom')


autolabel(rects1, "left")
autolabel(rects2, "right")

fig.tight_layout()

plt.show()