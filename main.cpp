#include <QDebug>
#include <QVector>
#include <QRect>
#include <QImage>
#include <QApplication>
#include <QWidget>
#include <QScreen>
#include <QDesktopWidget>
#include <QThread>
#include <windows.h>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

//std::string image_window = "Source Image"; //test
//std::string result_window = "Result window"; //test
int screenX = 0, screenY = 0, screenW = 640, screenH = 480;
int mouseX = 0, mouseY = 0;
int cRollRequest = 0, makeRequest = 0, onigiriRequest = 0, sRollRequest = 0, shrimpRequest = 0, unagiRequest = 0, dragonRequest = 0, comboRequest = 0;
int orderCount = 0;
int riceCount = 10, roeCount = 10, noriCount = 10, salmonCount = 5, shrimpCount = 5, unagiCount = 5;
int dishY = 0, dish1X = 0, dish2X = 0, dish3X = 0, dish4X = 0, dish5X = 0, dish6X = 0;
boolean failed = false;
boolean win = false;
std::string path = "X://GitHub/Project/SushiChef/images/";

class Match : public QRect{
    public:
        double matching_score;
};

QVector<Match> doMatching(cv::Mat img, cv::Mat templ, int max_matches, float matching_percent = 0.95) {

//    cv::namedWindow( image_window, CV_WINDOW_AUTOSIZE ); //test
//    cv::namedWindow( result_window, CV_WINDOW_AUTOSIZE ); //test

    cv::Mat result;
//    cv::Mat img_display; //test
    QVector<Match> matches;
//    img.copyTo( img_display ); //test

    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create( result_rows, result_cols, CV_32FC1 );
    matchTemplate( img, templ, result, 5);
    while (true){

        double min_val; double max_val; cv::Point min_loc; cv::Point max_loc; cv::Point match_loc;
        minMaxLoc( result, &min_val, &max_val, &min_loc, &max_loc, cv::Mat() );
        match_loc = max_loc;

        if((max_val > matching_percent) && (matches.size() != max_matches)){  //when the match has a value higher than 0.XX and matches to find hasnt been reached
            Match new_match;
            new_match.setLeft(match_loc.x);
            new_match.setTop(match_loc.y);
            new_match.setWidth(templ.cols);
            new_match.setHeight(templ.rows);
            new_match.matching_score = max_val;
            matches.append(new_match);

            qDebug() << "Matching number:" << matches.size() << new_match.left() << new_match.top() << new_match.matching_score;
            floodFill(result, cv::Point(match_loc.x,match_loc.y), cv::Scalar(0,0,0), 0, cv::Scalar(10,10,10));
//            rectangle( img_display, match_loc, cv::Point( match_loc.x + templ.cols , match_loc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 ); //test
//            imshow( image_window, img_display ); //test
//            imshow( result_window, result ); //test
            cv::waitKey(0); //Check Match by Match by pressing a button
        }
        else
        {
            if (matches.size() == max_matches){
                qDebug() << matches.size() << "matches found, exit searching";
            }
            else {
            // qDebug() << "no more matches";
             }
        break;
        }
    }
    return matches;
}


cv::Mat screenshotRect(int x, int y, int w, int h) {
    QRect shot_rect(x, y, w, h);
    QImage desk = qApp->screens().at(0)->grabWindow(
    QDesktopWidget().winId(),
    shot_rect.left(),
    shot_rect.top(),
    shot_rect.width(),
    shot_rect.height()).toImage();
    desk = desk.convertToFormat( QImage::Format_RGB888 );
    desk = desk.rgbSwapped();
    return cv::Mat( desk.height(), desk.width(),
                    CV_8UC3,
                    const_cast<uchar*>(desk.bits()),
                    static_cast<size_t>(desk.bytesPerLine())
                    ).clone();
}

cv::Mat screenshot() {
    QImage desk = qApp->screens().at(0)->grabWindow(
    QDesktopWidget().winId()
    ).toImage();
    desk = desk.convertToFormat( QImage::Format_RGB888 );
    desk = desk.rgbSwapped();
    return cv::Mat( desk.height(), desk.width(),
                    CV_8UC3,
                    const_cast<uchar*>(desk.bits()),
                    static_cast<size_t>(desk.bytesPerLine())
                    ).clone();
}

cv::Mat screenshotGame() {
    QRect shot_rect(screenX, screenY, screenW, screenH);
   // qDebug() << screenX << screenY << screenW << screenH;
    QImage desk = qApp->screens().at(0)->grabWindow(
    QDesktopWidget().winId(),
    shot_rect.left(),
    shot_rect.top(),
    shot_rect.width(),
    shot_rect.height()).toImage();
    desk = desk.convertToFormat( QImage::Format_RGB888 );
    desk = desk.rgbSwapped();
    return cv::Mat( desk.height(), desk.width(),
                    CV_8UC3,
                    const_cast<uchar*>(desk.bits()),
                    static_cast<size_t>(desk.bytesPerLine())
                    ).clone();
}

void mouseClick(int x, int y){
    QCursor mouse;
    mouse.setPos(x + screenX,y + screenY);
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 1, 1, 0, 0);
}

void getPlayScreen() {
    QVector<Match> matches = doMatching(screenshot(), cv::imread(path + "topLeft.png", 1), 1);
    if (matches.size() > 0) {
        screenX = matches[0].left();
        screenY = matches[0].top();
    } else {
        qDebug() << "Sushi Go Round not found";
        exit(0);
    }
}

void waitPosImage(std::string templName, int max_matches) {
    QVector<Match> matches;
    do {
        QVector<Match> matches_temp = doMatching(screenshotGame(), cv::imread(path + templName), max_matches);
        matches = matches_temp;
    } while(matches.size() < 1);
    mouseX = matches[0].left();
    mouseY = matches[0].top();
}

void waitPosImageClick(std::string templName, int max_matches, float matching_percent = 0.95) {
    QVector<Match> matches;
    do {
        QVector<Match> matches_temp = doMatching(screenshotGame(), cv::imread(path + templName), max_matches, matching_percent);
        matches = matches_temp;
    } while(matches.size() < 1);
    qDebug() << "click on" << QString::fromStdString(templName)  << matches[0].left() << matches[0].top();
    mouseClick(matches[0].left(), matches[0].top());
    QThread::msleep(10);
}

void start() {
    waitPosImageClick("playButton.png", 1);
    waitPosImageClick("continue.png", 1);
    waitPosImageClick("continue2.png", 1);
    waitPosImageClick("skip.png", 1);
    waitPosImageClick("continue.png", 1);
}

void winCheck() {
   cv::Mat screen = screenshotGame();
   QVector<Match> failedMatch = doMatching(screen, cv::imread(path + "failed.png"), 6);
   QVector<Match> winMatch = doMatching(screen, cv::imread(path + "win.png"), 6);
   if (failedMatch.size() == 1) {
       failed = true;
   }
   if (winMatch.size() == 1) {
       win = true;
   }
}

void getDishesPos() {
        QVector<Match> table = doMatching(screenshotGame(), cv::imread(path + "table.png"), 6);
        if (table.size() == 6) {
            dishY = table[0].top();
            dish1X = table[0].left() + 40;
            dish2X = table[1].left() + 40;
            dish3X = table[2].left() + 40;
            dish4X = table[3].left() + 40;
            dish5X = table[4].left() + 40;
            dish6X = table[5].left() + 40;
        }
        qDebug() << "getting dishes"<< dishY << dish1X << dish2X << dish3X << dish4X << dish5X << dish6X;
}

void getDishes() {
    qDebug() << "getting dishes"<< dishY << dish1X << dish2X << dish3X << dish4X << dish5X << dish6X;
    mouseClick(dish1X, dishY);
    mouseClick(dish2X, dishY);
    mouseClick(dish3X, dishY);
    mouseClick(dish4X, dishY);
    mouseClick(dish5X, dishY);
    mouseClick(dish6X, dishY);
}

void getOrder(int minOrders = 1) {
    do {
        qDebug() << "getting Orders";
        cv::Mat screen = screenshotGame();
        QVector<Match> cRollRequestMatch = doMatching(screen, cv::imread(path + "cRollRequest.png"), 6);
        QVector<Match> makeRequestMatch = doMatching(screen, cv::imread(path + "makeRequest.png"), 6);
        QVector<Match> onigiriRequestMatch = doMatching(screen, cv::imread(path + "onigiriRequest.png"), 6);
        QVector<Match> sRollRequestMatch = doMatching(screen, cv::imread(path + "sRollRequest.png"), 6);
        QVector<Match> shrimpRequestMatch = doMatching(screen, cv::imread(path + "shrimpRequest.png"), 6);
        QVector<Match> unagiRequestMatch = doMatching(screen, cv::imread(path + "unagiRequest.png"), 6);
        QVector<Match> dragonRequestMatch = doMatching(screen, cv::imread(path + "dragonRequest.png"), 6);
        QVector<Match> comboRequestMatch = doMatching(screen, cv::imread(path + "comboRequest.png"), 6);
        cRollRequest = cRollRequestMatch.size(), makeRequest = makeRequestMatch.size(), shrimpRequest = shrimpRequestMatch.size(),
        onigiriRequest = onigiriRequestMatch.size(), sRollRequest = sRollRequestMatch.size(), unagiRequest = unagiRequestMatch.size(),
        dragonRequest = dragonRequestMatch.size(), comboRequest = comboRequestMatch.size();
        orderCount = cRollRequestMatch.size() + makeRequestMatch.size() + onigiriRequestMatch.size() + sRollRequestMatch.size() +
        shrimpRequestMatch.size() + unagiRequestMatch.size() + dragonRequestMatch.size() + comboRequestMatch.size();
        getDishes();
        winCheck();
    } while (orderCount < minOrders && win == false && failed == false);
}

void orderSupplies() {
    qDebug() << "order supplies";
    if (riceCount < 2) {
        waitPosImageClick("order.png", 1);
        waitPosImageClick("riceMenu.png", 1);
        mouseClick(screenX, screenY);
        QThread::msleep(50);
        waitPosImageClick("riceOrder.png", 1);
        waitPosImageClick("standardDelivery.png", 1);
        riceCount = riceCount + 10;
    }
    if (roeCount < 2) {
        waitPosImageClick("order.png", 1);
        waitPosImageClick("toppingMenu.png", 1, 0.98);
        mouseClick(screenX, screenY);
        QThread::msleep(50);
        waitPosImageClick("roeOrder.png", 1);
        waitPosImageClick("standardDelivery.png", 1);
        roeCount = roeCount + 10;
    }
    if (noriCount < 1) {
        waitPosImageClick("order.png", 1);
        waitPosImageClick("toppingMenu.png", 1);
        mouseClick(screenX, screenY);
        QThread::msleep(50);
        waitPosImageClick("noriOrder.png", 1);
        waitPosImageClick("standardDelivery.png", 1);
        noriCount = noriCount + 10;
    }
    if (salmonCount < 2) {
        waitPosImageClick("order.png", 1);
        waitPosImageClick("toppingMenu.png", 1);
        mouseClick(screenX, screenY);
        QThread::msleep(50);
        waitPosImageClick("salmonOrder.png", 1, 0.98);
        waitPosImageClick("standardDelivery.png", 1);
        salmonCount = salmonCount + 5;
    }
    if (shrimpCount < 2) {
        waitPosImageClick("order.png", 1);
        waitPosImageClick("toppingMenu.png", 1);
        mouseClick(screenX, screenY);
        QThread::msleep(50);
        waitPosImageClick("shrimpOrder.png", 1, 0.98);
        waitPosImageClick("standardDelivery.png", 1);
        shrimpCount = shrimpCount + 5;
    }
    if (unagiCount < 2) {
        waitPosImageClick("order.png", 1);
        waitPosImageClick("toppingMenu.png", 1);
        mouseClick(screenX, screenY);
        QThread::msleep(50);
        waitPosImageClick("unagiOrder.png", 1, 0.98);
        waitPosImageClick("standardDelivery.png", 1);
        unagiCount = unagiCount + 5;
    }
}

void makeSushi() {
    qDebug() << "making sushi";
    if (comboRequest > 0) {
        getDishes();
        for (int a = 0; a < comboRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("unagiDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("roeDesk.png", 1);
            waitPosImageClick("salmonDesk.png", 1);
            waitPosImageClick("shrimpDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 2, noriCount = noriCount -1, unagiCount = unagiCount - 1, salmonCount = salmonCount - 1, shrimpCount = shrimpCount - 1, roeCount = roeCount - 1;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (dragonRequest > 0) {
        getDishes();
        for (int a = 0; a < dragonRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("unagiDesk.png", 1);
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("roeDesk.png", 1);
            waitPosImageClick("unagiDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 2, noriCount = noriCount -1, unagiCount = unagiCount - 2, roeCount = roeCount - 1;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (unagiRequest > 0) {
        getDishes();
        for (int a = 0; a < unagiRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("unagiDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("unagiDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 1, noriCount = noriCount -1, unagiCount = unagiCount - 2;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (shrimpRequest > 0) {
        getDishes();
        for (int a = 0; a < shrimpRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("shrimpDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("shrimpDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 1, noriCount = noriCount -1, shrimpCount = shrimpCount - 2;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (sRollRequest > 0) {
        getDishes();
        for (int a = 0; a < sRollRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("salmonDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("salmonDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 1, noriCount = noriCount -1, salmonCount = salmonCount - 2;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (cRollRequest > 0) {
        getDishes();
        for (int a = 0; a < cRollRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("roeDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 1, noriCount = noriCount -1, roeCount = roeCount - 1;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (makeRequest > 0) {
        getDishes();
        for (int a = 0; a < makeRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("roeDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("roeDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 1, noriCount = noriCount -1, roeCount = roeCount - 2;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
    if (onigiriRequest > 0) {
        getDishes();
        for (int a = 0; a < onigiriRequest; a = a + 1) {
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("noriDesk.png", 1);
            waitPosImageClick("riceDesk.png", 1);
            waitPosImageClick("bambusRoll.png", 1);
            waitPosImage("bambusRollEmpty.png", 1);
            riceCount = riceCount - 2, noriCount = noriCount -1;
            orderSupplies();
            for(int a = 0; a < 5; a = a + 1) {
            getDishes();
            QThread::msleep(100);
            }
        }
    }
        onigiriRequest = 0;
        makeRequest = 0;
        cRollRequest = 0;
        sRollRequest = 0;
        shrimpRequest = 0;
        unagiRequest = 0;
        dragonRequest = 0;
        comboRequest = 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    getPlayScreen();
    doMatching(screenshotGame(), cv::imread(path + "topLeft.png", 1), 1);
    start();
    QThread::msleep(200);
    getDishesPos();
    getOrder(3);
    do {
        getOrder();
        makeSushi();
        winCheck();
        for (int a = 0; a < 20; a = a + 1) {
        getDishes();
        QThread::msleep(350);
        }
    } while (failed == false and win == false);
    do {
        if (win == true) {
            riceCount = 10, noriCount = 10, roeCount = 10, salmonCount = 5, unagiCount = 5, shrimpCount = 5;
            win = false;
            waitPosImageClick("continue3.png", 1);
            QThread::msleep(200);
            waitPosImageClick("continue3.png", 1);
            getOrder(3);
            do {
                getOrder();
                makeSushi();
                for (int a = 0; a < 20; a = a + 1) {
                getDishes();
                QThread::msleep(350);
                }
            } while (failed == false and win == false);
        }
    } while (win == true);
    cv::waitKey(0);
    return 0;
}

