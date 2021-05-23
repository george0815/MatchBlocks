#ifndef BLOCKMATCHER_H
#define BLOCKMATCHER_H

#include <QMainWindow>
#include <QListWidgetItem>


using namespace std;


//block struct that holds average red, blue, and green valuess and the percentage of transparent pixels
extern struct block{

    string name;
    int r, g, b, rgbRange;
    double a;




};


//holds all blocks
extern vector<block> blocks;

QT_BEGIN_NAMESPACE
namespace Ui { class BlockMatcher; }
QT_END_NAMESPACE

class BlockMatcher : public QMainWindow
{
    Q_OBJECT

public:
    BlockMatcher(QWidget *parent = nullptr);
    ~BlockMatcher();



private slots:

    //used to sort blocks strcut vector by rgb difference
    static bool sortByRgbDiff(const block& a, const block& b);

    //imports block textures and gets pixel data
    void on_importData_clicked();

    //Gets and displays blocks with average color closest to RGB value in spin boxes
    void displayByRGB();

    //calls displayByRGB() upon value change
    void on_R_valueChanged();

    //calls displayByRGB() upon value change
    void on_G_valueChanged();

    //calls displayByRGB() upon value change
    void on_B_valueChanged();

    //parses block color data from text file
    void parseBlockData();

    //calls displayByRGB() upon being clicked
    void on_displayBlocks_clicked();

    //checks that the amount of blocks shown doen't exceed the amount of valid textures, if it doesnt then calls displaysByRGB
    void on_blockAmountShown_valueChanged();

    //sets filter for alpha and calls displaysByRGB upon change
    void on_alphaFilter_valueChanged();

    //copies minecraft command to give blocks to clipboard upon item being clicked
    void on_blockDisplay_itemClicked(QListWidgetItem *item);

    //opens color pickers and sets spinbox values to the rgb values of the color picked
    void on_getColor_clicked();

private:
    Ui::BlockMatcher *ui;
};
#endif // BLOCKMATCHER_H
