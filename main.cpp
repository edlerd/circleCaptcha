#include "EasyBMP.h"
#include <set>
using namespace std;

struct kreis
{
    int x;
    int y;
    int radius;
    double verhaeltnis;
};

int main( int argc, char* argv[] )
{
    //Einlesen
    BMP Background;
    Background.ReadFromFile("captcha3.bmp");

    int width=Background.TellWidth();
    int height=Background.TellHeight();

    //schreibe die graustufen aus dem bitmap in color array
    int color[width][height];

    for (int x=0; x<width; x++)
    for (int y=0; y<height;  y++)
    {
        color[x][y] = (int)Background.GetPixel(x,y).Blue;
    }

    //kreise mit verschiedenen radien und möglichen x,y werten durchprobieren
    double verhaeltnis,minVerhaeltnis=1;
    int found;
    kreis myKreise[100];
    int anzahlKreise=0;
    int farbGrenzeZuWeiss=240;
    for (int radius=10; radius<27; radius++)
    {
        for (int x=radius+1; x<width-radius-1; x++)
        {
            for (int y=radius+1; y<height-radius-1; y++)
            {
                int weisseFelder=0;
                int anzahlFelder=0;

                //durchlaufe alle felder des aktuellen kreises und prüfe ob das feld (oder benachbarte felder im weiß bereich liegen
                for (int kreisx=x-radius; kreisx<x+radius; kreisx++)
                {
                    anzahlFelder++;
                    int kreisy1 = floor(y+sqrt(radius*radius-(kreisx-x)*(kreisx-x)));
                    if (color[kreisx][kreisy1]>farbGrenzeZuWeiss or color[kreisx][kreisy1+1]>farbGrenzeZuWeiss )
                        weisseFelder++;
                    int kreisy2 = floor(y-sqrt(radius*radius-(kreisx-x)*(kreisx-x)));
                    if (kreisy2!=kreisy1)
                    {
                        anzahlFelder++;
                        if (color[kreisx][kreisy2]>farbGrenzeZuWeiss or color[kreisx][kreisy2+1]>farbGrenzeZuWeiss)
                            weisseFelder++;
                    }
                }

                //verhaeltnis weisser zu nichtweisser felder
                verhaeltnis = (double)weisseFelder/anzahlFelder;

                //da ist ein weißer kreis!
                if (verhaeltnis>0.6)
                {
                    //speichere diesen kreis in array
                    myKreise[anzahlKreise].x=x;
                    myKreise[anzahlKreise].y=y;
                    myKreise[anzahlKreise].radius=radius;
                    myKreise[anzahlKreise].verhaeltnis=verhaeltnis;
                    anzahlKreise++;
                }
            }
        }
    }

    //filtere doppelt gefundene kreise, behalte die mit dem hoechsten verhaeltnis
    kreis gefilterteKreise[100];
    int anzahlGefilterteKreise=0;
    set<int> dontadd;
    for (int i=0; i<anzahlKreise; i++)
    {
        int addthis=i;

        //noch nicht gefilterter kreis?
        if (dontadd.find(i)==dontadd.end())
        {
            //vergleiche den kreis mit allen anderen, welche sind identisch?
            for (int filter=0; filter<anzahlKreise; filter++)
            {
                //ein eintrag der denselben kreis beschreibt?
                if ( (myKreise[i].x==myKreise[filter].x           or myKreise[i].x==myKreise[filter].x+1           or myKreise[i].x==myKreise[filter].x+2           or myKreise[i].x==myKreise[filter].x-1						or myKreise[i].x==myKreise[filter].x-2) and
                     (myKreise[i].y==myKreise[filter].y           or myKreise[i].y==myKreise[filter].y+1           or myKreise[i].y==myKreise[filter].y+2           or myKreise[i].y==myKreise[filter].y-1						or myKreise[i].y==myKreise[filter].y-2) and
                     (myKreise[i].radius==myKreise[filter].radius or myKreise[i].radius==myKreise[filter].radius+1 or myKreise[i].radius==myKreise[filter].radius+2 or myKreise[i].radius==myKreise[filter].radius-1 or myKreise[i].radius==myKreise[filter].radius-2 or myKreise[i].radius==myKreise[filter].radius+3 or myKreise[i].radius==myKreise[filter].radius-3)
                   )
                {
                    //diesen kreis nicht nochmal filtern
                    dontadd.insert(filter);

                    //dieser kreis ist deutlicher zu erkennen (mehr weiß pro kreis)
                    if (myKreise[addthis].verhaeltnis < myKreise[filter].verhaeltnis)
                    {
                        addthis=filter;
                    }
                }
            }

            //speichere den besten kreis in gefilterteKreise array
            gefilterteKreise[anzahlGefilterteKreise].x=myKreise[addthis].x;
            gefilterteKreise[anzahlGefilterteKreise].y=myKreise[addthis].y;
            gefilterteKreise[anzahlGefilterteKreise].verhaeltnis=myKreise[addthis].verhaeltnis;
            gefilterteKreise[anzahlGefilterteKreise].radius=myKreise[addthis].radius;
            anzahlGefilterteKreise++;
        }
    }

    //durchlaufe die gefilterten Kreise
    for (int i=0; i<anzahlGefilterteKreise; i++)
    {
        cout << "kreis gefunden " << gefilterteKreise[i].radius << " " << gefilterteKreise[i].x << " " << gefilterteKreise[i].y << " " << gefilterteKreise[i].verhaeltnis << endl;

        //dieser kreis hat das kleinste verhaeltnis => wahrscheinlich ist es der geöffnete.
        if (gefilterteKreise[i].verhaeltnis<minVerhaeltnis)
        {
            minVerhaeltnis=gefilterteKreise[i].verhaeltnis;

            //codiere gefundene pixel in found
            if (gefilterteKreise[i].x<100)
                found = 900+gefilterteKreise[i].x;
            else
                found = gefilterteKreise[i].x;

            found *=1000;
            found += gefilterteKreise[i].y;
        }

    }

    cout << found;
    return found;
}
