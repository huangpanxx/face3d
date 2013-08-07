#ifndef MATTING_H
#define MATTING_H

#include <QImage>

namespace matting {
class Matting {
public:
    Matting(const QImage &foreground, const QImage &background, const QImage &alpha) {
        this->forground = foreground;
        this->background = background;
        this->alpha = alpha;
    }
    QImage forground;
    QImage background;
    QImage alpha;
};

Matting getMatting(const QImage &image, const QImage &scribble,
                  QRgb foreground, QRgb background,
                  int w=1, double e=1e-7, double lambda=1e5);
}

#endif // MATTING_H
