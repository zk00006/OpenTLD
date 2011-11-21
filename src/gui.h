
#ifndef GUI_H_
#define GUI_H_

#include <string>

#include <opencv/highgui.h>

namespace tld {

class Gui {
public:
	Gui();
	~Gui();
	void init();
	void showImage(IplImage * image);
	char getKey();
	std::string windowName();

private:
	std::string m_window_name;
};

/**
 * Get a bounding box from the user.
 * @param img image to display
 * @param rect CvRect containing the coordinates of the bounding box
 * @param gui initialized gui
 * @return PROGRAM_EXIT if 'q' or 'Q' pressed, SUCCESS if everything went right
 */
int getBBFromUser(IplImage * img, CvRect & rect, Gui * gui);

}

#endif /* GUI_H_ */
