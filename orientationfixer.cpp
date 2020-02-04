#include "orientationfixer.h"

OrientationFixer::OrientationFixer()
{

}

void OrientationFixer::setParams(const PREPROCESSING_RESULTS &input, const QVector<MINUTIA> &minutiae, const QVector<MINUTIA> &invertedMinutiae)
{
    this->input = input;
    this->minutiae = minutiae;
    this->invertedMinutiae = invertedMinutiae;
}

void OrientationFixer::fix()
{
    this->fixedMinutiae = this->minutiae;

    this->fixEndings();
    if (this->input.imgSkeletonInverted.cols > 0 && this->input.imgSkeletonInverted.rows > 0) {
        this->fixBifurcations();
    }
    else{
        emit this->extractionErrorSignal(21);
    }

}

void OrientationFixer::finalizeDirections(QVector<MINUTIA> &minutiae)
{
    for (MINUTIA &minutia : minutiae) {
        if (minutia.type == 0) {
            if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
            else minutia.angle += M_PI;
        }

        if (minutia.angle <= M_PI) minutia.angle = M_PI - minutia.angle;
        else minutia.angle = 3 * M_PI - minutia.angle;
    }
}

void OrientationFixer::fixEndings()
{
    int centerPixel = -1;
    int incomingPixel = -1;

    for (MINUTIA &minutia : this->fixedMinutiae) {
        // len ukoncenia
        if (minutia.type == 0) {

            // ukoncenie prichadza z laveho horneho rohu
            // x 0 0
            // 0 x 0
            // 0 0 0
            centerPixel = this->input.imgSkeleton.at<uchar>(minutia.xy.y(), minutia.xy.x());
            incomingPixel = this->input.imgSkeleton.at<uchar>(minutia.xy.y() - 1, minutia.xy.x() - 1);
            if (centerPixel == 0 && incomingPixel == 0) {
                if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                else minutia.angle += M_PI;
            }

            // ukoncenie prichadza zhora
            // 0 x 0
            // 0 x 0
            // 0 0 0
            incomingPixel = this->input.imgSkeleton.at<uchar>(minutia.xy.y() - 1, minutia.xy.x());
            if(centerPixel == 0 && incomingPixel == 0){
                if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                else minutia.angle += M_PI;
            }

            // ukoncenie prichadza z praveho horneho rohu
            // 0 0 x
            // 0 x 0
            // 0 0 0
            incomingPixel = this->input.imgSkeleton.at<uchar>(minutia.xy.y() - 1, minutia.xy.x() + 1);
            if(centerPixel == 0 && incomingPixel == 0){
                if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                else minutia.angle += M_PI;
            }

            // ukoncenie prichadza zlava
            // 0 0 0
            // x x 0
            // 0 0 0
            incomingPixel = this->input.imgSkeleton.at<uchar>(minutia.xy.y(), minutia.xy.x() - 1);
            if((centerPixel == 0 && incomingPixel == 0)){
                if(minutia.angle<=M_PI_2){
                    if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                    else minutia.angle += M_PI;
                }
            }

            // ukoncenie prichadza zprava
            // 0 0 0
            // 0 x x
            // 0 0 0
            incomingPixel = this->input.imgSkeleton.at<uchar>(minutia.xy.y(), minutia.xy.x() + 1);
            if((centerPixel == 0 && incomingPixel == 0)){
                if(minutia.angle>M_PI_2){
                    if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                    else minutia.angle += M_PI;
                }
            }
        }
    }
}

void OrientationFixer::fixBifurcations()
{
    int centerPixel = -1;
    int incomingPixel = -1;

    for (MINUTIA &minutia : this->fixedMinutiae) {
        // len vidlice
        if (minutia.type == 1) {

            MINUTIA closestMinutia;
            qreal minLength = 999999;
            qreal currentLength = -1;

            for(MINUTIA invertedMinutia : invertedMinutiae) {
                if (invertedMinutia.type == 0) {
                    QLineF line(QPointF(minutia.xy.x(), minutia.xy.y()), QPointF(invertedMinutia.xy.x(), invertedMinutia.xy.y()));
                    currentLength = line.length();
                    if(currentLength < minLength){
                        minLength = currentLength;
                        closestMinutia = invertedMinutia;
                    }
                }
            }

            // 0 0 0
            // 0 x 0
            // x 0 0
            centerPixel = this->input.imgSkeletonInverted.at<uchar>(closestMinutia.xy.y(), closestMinutia.xy.x());
            incomingPixel = this->input.imgSkeletonInverted.at<uchar>(closestMinutia.xy.y() + 1, closestMinutia.xy.x() - 1);
            if (centerPixel == 0 && incomingPixel == 0) {
                if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                else minutia.angle += M_PI;
            }

            // 0 0 0
            // 0 x 0
            // 0 x 0
            incomingPixel = this->input.imgSkeletonInverted.at<uchar>(closestMinutia.xy.y() + 1, closestMinutia.xy.x());
            if (centerPixel == 0 && incomingPixel == 0) {
                if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                else minutia.angle += M_PI;
            }


            // 0 0 0
            // 0 x 0
            // 0 0 x
            incomingPixel = this->input.imgSkeletonInverted.at<uchar>(closestMinutia.xy.y() + 1, closestMinutia.xy.x() + 1);
            if (centerPixel == 0 && incomingPixel == 0) {
                if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                else minutia.angle += M_PI;
            }

            // 0 0 0
            // x x 0
            // 0 0 0
            incomingPixel = this->input.imgSkeletonInverted.at<uchar>(closestMinutia.xy.y(), closestMinutia.xy.x() - 1);
            if ((centerPixel == 0 && incomingPixel == 0)) {
                if (minutia.angle > M_PI_2) {
                    if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                    else minutia.angle += M_PI;
                }
            }

            // 0 0 0
            // 0 x x
            // 0 0 0
            incomingPixel = this->input.imgSkeletonInverted.at<uchar>(closestMinutia.xy.y(), closestMinutia.xy.x()+1);
            if ((centerPixel == 0 && incomingPixel == 0)) {
                if (minutia.angle <= M_PI_2) {
                    if (minutia.angle+M_PI > 2*M_PI) minutia.angle = minutia.angle+M_PI - 2*M_PI;
                    else minutia.angle += M_PI;
                }
            }
        }
    }
}

QVector<MINUTIA> OrientationFixer::getFixedMinutiae() const
{
    return fixedMinutiae;
}
