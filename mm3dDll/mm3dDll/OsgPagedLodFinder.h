#pragma once
#include <vector>
#include "osg/NodeVisitor"

class COsgPagedLodFinder : public osg::NodeVisitor
{
public:
	COsgPagedLodFinder(void);
	~COsgPagedLodFinder(void);

	//�ӿ�
	void FindPagedLod(osg::ref_ptr<osg::Node> node, std::vector<osg::PagedLOD*> &vecPagedLod);			//����pagedLod

public:
	std::vector<osg::PagedLOD*> vecPagedLod;

protected:
	virtual void apply(osg::PagedLOD& pagedLod);
};

