#include "blockmatcher.h"
#include "ui_blockmatcher.h"
#include <qfiledialog.h>
#include <string>
#include <QDebug>
#include <set>
#include <QMessageBox>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QClipboard>
#include <QColorDialog>
#include <QColor>



using namespace std;

struct block;

vector<block> blocks;


BlockMatcher::BlockMatcher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BlockMatcher)
{
    ui->setupUi(this);










    //sets the size
    setFixedSize(571, 340);


    //checks for blocks.cfg if it doesn't then prompts user to import data
    QFileInfo blockCheck(QString::fromStdString("blocks.cfg"));

    if(!(blockCheck.exists())){

    QMessageBox::information(this, "Notice", "Textures have to be imported. \nTo import textures, extract the files from the minecraft version jar (usually found in C\\Users\\USERNAME\\AppData\\Roaming\\.minecraft\\versions\\MINECRAFTVERSION). \nThen select the \"block\" folder in MINECRAFTVERSION\\assets\\minecraft\\textures.");

    bool isFolderValid = false;

    while (isFolderValid == false){


        //open file dialog so user can choose block texture folder
        QFileDialog dialog;

        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly);

        QString path = dialog.getExistingDirectory(this, "Open Image Folder");



        QDir dir(path);

        QDir().mkdir(".\\textures\\");



        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);



        //for counting how many files are valid(texture)
        int i = 0;









        //checks how many valid files/textures there are in the folder
        foreach (QFileInfo fileInfo, dir.entryInfoList()) {

                string ext = fileInfo.suffix().toLocal8Bit().constData();



                if(ext == "png"){

                   i++;

                }

            }


        //if there are valid files, proceed, if not, then throw error
        if(i > 0){


        //prevents next loop
        isFolderValid = true;




        //creates txt file of block data
        ofstream blockData;
        blockData.open("blocks.cfg");


        //for each texture
        foreach (QFileInfo fileInfo, dir.entryInfoList()) {



                string ext = fileInfo.suffix().toLocal8Bit().constData();

                if(ext == "png"){


                //copy file to folder
                QFile::copy(fileInfo.filePath(), ("./textures/" + fileInfo.fileName()));


                //gets average color
                int r = 0, g = 0, b = 0;
                double a = 0;




                //gets the average color of block via adding rgb value of each pixel and dividing by 256 (16 x 16)
                QImage pix("./textures/" + fileInfo.fileName());

                //holds number of pixels that have an alpha values of 0
                double alphaAmount = 0;


                //gets color data for texture
                for (int j = 0; j < 16; j++){


                    for (int k = 0; k < 16; k++){

                        r += pix.pixelColor(j, k).red();
                        g += pix.pixelColor(j, k).green();
                        b += pix.pixelColor(j, k).blue();




                        if(pix.pixelColor(j, k).alpha() > 0){

                            alphaAmount++;

                        }


                    }


                }

                r = r/256;
                g = g/256;
                b = b/256;
                a = alphaAmount/256;











                //puts data into vectors and exports into into txt
                block tempBlock;

                tempBlock.name = fileInfo.fileName().toLocal8Bit().constData();
                blockData << tempBlock.name << endl;

                tempBlock.r = r;
                blockData << tempBlock.r << endl;

                tempBlock.g = g;
                blockData << tempBlock.g << endl;

                tempBlock.b = b;
                blockData << tempBlock.b << endl;           

                tempBlock.a = a;
                blockData << tempBlock.a << endl;

                blocks.push_back(tempBlock);





                }

            }


        //close streaming to block data file
        blockData.close();

         }


        else{

            QMessageBox err(QMessageBox::Critical, tr("Error"), tr("No valid images in folder."));
            err.setWindowFlags(err.windowFlags() | Qt::WindowStaysOnTopHint);

            err.exec();

            }





    }
    }

    //parses block data and displays blocks
    else{

    parseBlockData();

    }



    //if the amount of blocks shown exceeds the amount of valid textures, sets amount of blocks displayed to the amount of valid textures
    if(ui->blockAmountShown->value() > blocks.size()){

        ui->blockAmountShown->setValue(ui->blockAmountShown->value() - (ui->blockAmountShown->value() - blocks.size()));

    }


    displayByRGB();

}

BlockMatcher::~BlockMatcher()
{
    delete ui;
}





//Gets and displays blocks with average color closest to RGB value in spin boxes
void BlockMatcher::displayByRGB(){



    //clears list so previous blocks arent displayed
    ui->blockDisplay->clear();



    //gets spinbox values to plug into formula
    int r1 = ui->R->value(), g1 = ui->G->value(), b1 = ui->B->value();
    int r2 = 0, g2 = 0, b2 = 0;




    //gets the difference between rgb values of every block compared to value from spin boxes
    for(int i  = 0 ; i < blocks.size(); i++){

        //gets block values to plug into formula
        r2 = blocks[i].r;
        g2 = blocks[i].g;
        b2 = blocks[i].b;


        //uses 3-Dimensional Pythagorean Theorem to get difference
        blocks[i].rgbRange = sqrt(pow((r2-r1), 2) + pow((g2-g1), 2) + pow((b2-b1), 2));




    }




    //sorts blocks vector by difference of block rgb value compared to value from the spinboxes
    sort(blocks.begin(), blocks.end(), BlockMatcher::sortByRgbDiff);



    //displays blocks that have an average color closest to the rgb value from the spinboxes
    int i = 0, j = 0;

    while (i < ui->blockAmountShown->value()){

        //if the percentage of transparent pixels is greater then the filteer value, then display block
        if(blocks[j].a >= ui->alphaFilter->value()){


        //holds path to texture
        QString texPath = QDir::currentPath() + "/textures/" + QString::fromStdString(blocks[j].name);


        //creates image from texture
        QImage img(texPath);




        //crops textures that are bigger than 16x16
        QRect cropArea(0, 0, 16, 16);
        img = img.copy(cropArea);



        //scales texture
        QPixmap texPixmap;
        texPixmap = texPixmap.fromImage(img.scaled(100, 100, Qt::IgnoreAspectRatio,Qt::FastTransformation));



        //creates block entry for list widget
        QListWidgetItem *block = new QListWidgetItem;


        //formats and sets tool tip
        QString blockNameNoExt(QString::fromStdString(blocks[j].name));
        blockNameNoExt.truncate(blockNameNoExt.lastIndexOf(QChar('.')));
        blockNameNoExt.replace("_", " ");
        block->setToolTip(blockNameNoExt);




        //sets icon for block entry
        ui->blockDisplay->setViewMode(QListView::IconMode);
        ui->blockDisplay->setIconSize(QSize(100, 100));
        block->setIcon(QIcon(texPixmap));


        //displays block entry in list widget
        ui->blockDisplay->addItem(block);



        i++;

        }


        //prevents infinite loop in case the amount of blocks displayed cant be achieved
        j++;
        if(j >= blocks.size()){
            break;
        }


      }

    //sets style sheet for color picked
    QString colorPickedSheet = QString::fromStdString("background-color: rgb(" + to_string(r1)+","+ to_string(g1)+"," + to_string(b1) +");");
    ui->colorPicked->setStyleSheet(colorPickedSheet);

    }

//imports block textures and gets pixel data
void BlockMatcher::on_importData_clicked()
{



    //open file dialog so user can choose block texture folder
    QFileDialog dialog;

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    QString path = dialog.getExistingDirectory(this, "Open Image Folder");



    QDir dir(path);

    QDir().mkdir(".\\textures\\");



    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);



    //for counting how many files are valid(texture)
    int i = 0;








    //checks how many valid files/textures there are in the folder
    foreach (QFileInfo fileInfo, dir.entryInfoList()) {

            string ext = fileInfo.suffix().toLocal8Bit().constData();



            if(ext == "png"){

               i++;

            }

        }


    //if there are valid files, proceed, if not, then throw error
    if(i > 0){






    //creates txt file of block data
    ofstream blockData;
    blockData.open("blocks.cfg");


    //clears block vector to prevent duplicates
    blocks.clear();


    //for each texture
    foreach (QFileInfo fileInfo, dir.entryInfoList()) {






            string ext = fileInfo.suffix().toLocal8Bit().constData();

            if(ext == "png"){


            //copy file to folder
            QFile::copy(fileInfo.filePath(), ("./textures/" + fileInfo.fileName()));


            //gets average color
            int r = 0, g = 0, b = 0;
            double a = 0;




            //gets the average color of block via adding rgb value of each pixel and dividing by 256 (16 x 16)
            QImage pix("./textures/" + fileInfo.fileName());

            //holds number of pixels that have an alpha values of 0
            double alphaAmount = 0;


            //gets color data for texture
            for (int j = 0; j < 16; j++){


                for (int k = 0; k < 16; k++){

                    r += pix.pixelColor(j, k).red();
                    g += pix.pixelColor(j, k).green();
                    b += pix.pixelColor(j, k).blue();




                    if(pix.pixelColor(j, k).alpha() > 0){

                        alphaAmount++;

                    }


                }


            }


            //sets color data
            r = r/256;
            g = g/256;
            b = b/256;
            a = alphaAmount/256;





            //puts data into vectors and exports into into txt
            block tempBlock;

            tempBlock.name = fileInfo.fileName().toLocal8Bit().constData();
            blockData << tempBlock.name << endl;

            tempBlock.r = r;
            blockData << tempBlock.r << endl;

            tempBlock.g = g;
            blockData << tempBlock.g << endl;

            tempBlock.b = b;
            blockData << tempBlock.b << endl;

            tempBlock.a = a;
            blockData << tempBlock.a << endl;

            blocks.push_back(tempBlock);






            }

        }


    //close streaming to block data file
    blockData.close();


    //if the amount of blocks shown exceeds the amount of valid textures, sets amount of blocks displayed to the amount of valid textures
    if(ui->blockAmountShown->value() > blocks.size()){

        ui->blockAmountShown->setValue(ui->blockAmountShown->value() - (ui->blockAmountShown->value() - blocks.size()));

    }
    else{
       ui->blockAmountShown->setValue(16);
    }

    //calls displayByRGB to display the newly importedd blocks
    displayByRGB();

     }


    else{
            QMessageBox::information(this, "Error", "No valid images in folder.");
        }




}

//used to sort blocks struct vector by rgb difference
bool BlockMatcher::sortByRgbDiff(const block& a, const block& b) {

    return a.rgbRange < b.rgbRange;

}

//calls displayByRGB() upon value change
void BlockMatcher::on_R_valueChanged()
{

    displayByRGB();

}

//calls displayByRGB() upon value change
void BlockMatcher::on_G_valueChanged()
{

    displayByRGB();

}

//calls displayByRGB() upon value change
void BlockMatcher::on_B_valueChanged()
{

    displayByRGB();

}

//parses block color data from text file
void BlockMatcher::parseBlockData(){



    //gets the amount of lines in the file
    int numLines = 0;


    ifstream fileIn("blocks.cfg");
    string temp;

    while ( std::getline(fileIn, temp) ){

       ++numLines;

    }

    fileIn.close();


    //open text file
    //ifstream fileIn("blocks.cfg"); //block data file
    fileIn.open("blocks.cfg");




    block tempBlock;
    string tempColor;


    //take entire line as a string and put it into block struct
        for (int i = 0; i < numLines/5; i++){






            getline(fileIn, tempBlock.name);


            getline(fileIn, tempColor);
            tempBlock.r = stoi(tempColor);


            getline(fileIn, tempColor);
            tempBlock.g = stoi(tempColor);


            getline(fileIn, tempColor);
            tempBlock.b = stoi(tempColor);


            getline(fileIn, tempColor);
            tempBlock.a = stod(tempColor);


            blocks.push_back(tempBlock);

        }







}

//calls displayByRGB() upon being clicked
void BlockMatcher::on_displayBlocks_clicked()
{

 displayByRGB();

}

//checks that the amount of blocks shown doen't exceed the amount of valid textures, if it doesnt then calls displaysByRGB
void BlockMatcher::on_blockAmountShown_valueChanged()
{

    if(ui->blockAmountShown->value() > blocks.size()){

        //throws error if the amount of blocks shown exceeds the amount of valid textures then sets amount of blocks displayed to the amount of valid textures
        QMessageBox::information(this, "Notice", "Blocks displayed exceeds the amount of textures");
        ui->blockAmountShown->setValue(ui->blockAmountShown->value() - (ui->blockAmountShown->value() - blocks.size()));

    }
    else{
        displayByRGB();
    }


}

//sets filter for alpha and calls displaysByRGB upon change
void BlockMatcher::on_alphaFilter_valueChanged()
{
    displayByRGB();
}

//copies minecraft command to give blocks to clipboard upon item being clicked
void BlockMatcher::on_blockDisplay_itemClicked(QListWidgetItem *item)
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    //replaces spaces with underscores
    QString temp = item->toolTip().replace(" ","_");
    clipboard->setText("/give @p " + temp + " 10");


}

//opens color pickers and sets spinbox values to the rgb values of the color picked
void BlockMatcher::on_getColor_clicked()
{

    //picks color
    QColor color = QColorDialog::getColor(Qt::white,this);
        QPalette palette;
        palette.setColor(QPalette::Base,color);

        //sets spinbox values
        ui->R->setValue(color.red());
        ui->G->setValue(color.green());
        ui->B->setValue(color.blue());





}
