#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

#include <QVector3D>

#include <vector>
#include <memory>

struct Line
{
    inline Line(QVector3D a, QVector3D b) : p0(a), p1(b) {}

    inline float getLength() const { return (p1 - p0).length(); }

    QVector3D p0;
    QVector3D p1;
};

class EnglishFields
{
public:
    EnglishFields(const std::vector<std::vector<float> >& _terrainHeightMap);
    ~EnglishFields();

    void checkAvailableSpace();
    void threeFieldModel();

    inline std::vector<Line> getFieldBoundary() const { return m_fieldBoundary; }
    inline std::vector< std::vector<Line> > getFields() const { return m_fields; }


private:
    EnglishFields();

    std::shared_ptr< std::vector<std::vector<float> > > m_heightMapCopy;
    std::vector<Line> m_fieldBoundary;
    std::vector< std::vector<Line> > m_fields;


};

#endif // ENGLISHFIELDS_H
