#include "sbs2filter.h"

Sbs2Filter* Sbs2Filter::m_pInstance = 0;

Sbs2Filter* Sbs2Filter::New(int fbandLow_, int fbandHigh_, int order_, QObject *parent)
{
    if (!m_pInstance)
        m_pInstance = new Sbs2Filter(fbandLow_,fbandHigh_,order_,parent);

    return m_pInstance;
}

Sbs2Filter* Sbs2Filter::New2(int fbandLow_, int fbandHigh_, int order_,
                            int fbandLow2_, int fbandHigh2_, int order2_, QObject *parent)
{
    if (!m_pInstance){
        m_pInstance = new Sbs2Filter(fbandLow_,fbandHigh_,order_,
                                     fbandLow2_, fbandHigh2_, order2_ ,parent);
    }

    return m_pInstance;
}

Sbs2Filter::Sbs2Filter(int fbandLow_, int fbandHigh_, int order_, QObject *parent): QObject(parent),fbandLow(fbandLow_), fbandHigh(fbandHigh_), order(order_)
{
    hcoef2 = 0;
    loadFilter();
}

Sbs2Filter::Sbs2Filter(int fbandLow_, int fbandHigh_, int order_,
                       int fbandLow2_, int fbandHigh2_, int order2_, QObject *parent)
{
    fbandLow=fbandLow_;
    fbandHigh=fbandHigh_;
    order=order_;
    fbandLow2nd=fbandLow2_;
    fbandHigh2nd=fbandHigh2_;
    order2nd=order2_;
    hcoef2 = 0;
    hcoef2_2nd = 0;
    loadFilter();
    loadFilter2nd();
}


void Sbs2Filter::loadFilter()
{

    if (!(hcoef2 == 0))
    {
        delete hcoef2;
        hcoef2 = 0;
    }

    hcoef2 = new DTU::DtuArray2D<double>(1,order+1);


    QString filename;
    filename.append(Sbs2Common::getRootAppPath());
    filename.append("filtercoef_fband");
    filename.append(QString::number(fbandLow));
    filename.append("-");
    filename.append(QString::number(fbandHigh));
    filename.append("_norder");
    filename.append(QString::number(order));
    filename.append(".txt");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "sbs2-filter : ERROR: unable to find file : "<<filename;
        return;
    }

    int i=0;
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str = line.data();
        QStringList list1 = str.split("\t");
        for (int j = 0; j < list1.size(); j++)
        {
            (*hcoef2)[0][j] = list1.at(j).toDouble();

        }
        i++;
    }

}

void Sbs2Filter::loadFilter2nd()
{

    if (!(hcoef2_2nd == 0))
    {
        delete hcoef2_2nd;
        hcoef2_2nd = 0;
    }

    hcoef2_2nd = new DTU::DtuArray2D<double>(1,order2nd+1);


    QString filename;
    filename.append(Sbs2Common::getRootAppPath());
    filename.append("filtercoef_fband");
    filename.append(QString::number(fbandLow2nd));
    filename.append("-");
    filename.append(QString::number(fbandHigh2nd));
    filename.append("_norder");
    filename.append(QString::number(order2nd));
    filename.append(".txt");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "sbs2-filter : ERROR: unable to find file : "<<filename;
        return;
    }

    int i=0;
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str = line.data();
        QStringList list1 = str.split("\t");
        for (int j = 0; j < list1.size(); j++)
        {
            (*hcoef2_2nd)[0][j] = list1.at(j).toDouble();
        }
        i++;
    }

}

void Sbs2Filter::updateFilter(int order_, int fbandLow_, int fbandHigh_)
{
    order=order_;
    fbandHigh=fbandHigh_;
    fbandLow=fbandLow_;
    loadFilter();
}

void Sbs2Filter::updateFilter2nd(int order_, int fbandLow_, int fbandHigh_,
                              int order2_, int fbandLow2_, int fbandHigh2_)
{
    order=order_;
    fbandHigh=fbandHigh_;
    fbandLow=fbandLow_;
    order2nd=order2_;
    fbandHigh2nd=fbandHigh2_;
    fbandLow2nd=fbandLow2_;
    loadFilter();
    loadFilter2nd();
}

void Sbs2Filter::doFilter(DTU::DtuArray2D<double>* values, DTU::DtuArray2D<double>* returnValues)
{

    if (!(values->dim1() == returnValues->dim1())) //14 for emotiv
        return;

    if (!(values->dim2() == (order +1))) //columns of the values
        return;

    double yn;

    for (int j=0; j<values->dim1(); j++)
    {
        yn = 0.0;
        for (int k=0; k<values->dim2(); k++)
        {
            yn += (*hcoef2)[0][k] * ((*values)[j][k]);
        }
        (*returnValues)[j][0] = yn;
    }
}

void Sbs2Filter::doFilter2(DTU::DtuArray2D<double>* values, DTU::DtuArray2D<double>* returnValues, DTU::DtuArray2D<double>* returnValues2nd)
{

    if (!(values->dim1() == returnValues->dim1())) //14 for emotiv
        return;

    if (!(values->dim2() == (order +1))) //columns of the values
        return;

    double yn, yn2nd;

    for (int j=0; j<values->dim1(); j++)
    {
        yn = 0.0;
        yn2nd = 0.0;
        for (int k=0; k<values->dim2(); k++)
        {
            yn += (*hcoef2)[0][k] * ((*values)[j][k]);
            yn2nd += (*hcoef2_2nd)[0][k] * ((*values)[j][k]);
        }
        (*returnValues)[j][0] = yn;
        (*returnValues2nd)[j][0] = yn2nd;
    }
}

Sbs2Filter::~Sbs2Filter()
{
    if (!(hcoef2 == 0))
    {
        delete hcoef2;
        hcoef2 = 0;
    }
    if (!(hcoef2_2nd == 0)){
        delete hcoef2_2nd;
        hcoef2_2nd = 0;
    }
    m_pInstance = 0;

}
