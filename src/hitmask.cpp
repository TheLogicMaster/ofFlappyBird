#include "hitmask.h"

Hitmask::Hitmask() = default;

Hitmask::Hitmask(ofImage& image) {
    ofPixels pixels = image.getPixels();
    for (int x = 0; x < pixels.getWidth(); x++) {
        std::vector<bool> column = std::vector<bool>();
        for (int y = 0; y < pixels.getHeight(); y++) {
            //printf("X: %d, Y: %d, R: %d, G: %d, B: %d, A: %d, Hue: %f, Brightness: %f, Saturation: %f\n", x, y, pixels.getColor(x, y).r, pixels.getColor(x, y).g, pixels.getColor(x, y).b, pixels.getColor(x, y).a, pixels.getColor(x, y).getHue(), pixels.getColor(x, y).getBrightness(), pixels.getColor(x, y).getSaturation());
            column.push_back(pixels.getColor(x, y).a != 0);
        }
        mask.push_back(column);
    }

}

bool Hitmask::checkCollision(ofRectangle rect, ofRectangle otherRect, Hitmask& otherMask) {
    ofRectangle r = rect.getIntersection(otherRect);

    if (rect.width == 0 || rect.height == 0)
        return false;

    int x1 = r.x - rect.x;
    int y1 = r.y - rect.y;
    int x2 = r.x - rect.x;
    int y2 = r.y - rect.y;

    for (int x = 0; x < r.width; x++)
        for (int y = 0; y < r.height; y++) {
            // Temp fix for out of bounds exception, happens when running into side of pipe
            if (mask.size() <= x1 + x || otherMask.getMask().size() <= x2 + x || mask.at(x1 + x).size() <= y1 + y ||
                otherMask.getMask().at(x2 + x).size() <= y2 + y) {
                continue;
            }
            if (mask.at(x1 + x).at(y1 + y) && otherMask.getMask().at(x2 + x).at(y2 + y))
                return true;
        }

    return false;
}

std::vector<std::vector<bool>> Hitmask::getMask() {
    return mask;
}

void Hitmask::verticalFlip() {
    for (auto &column: mask)
        reverse(column.begin(), column.end());
}
