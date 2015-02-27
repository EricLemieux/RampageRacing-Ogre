#include "OBJ.h"

OBJ::OBJ()
{

}
OBJ::OBJ(Ogre::String fileName)
{
	Load(fileName);
}
OBJ::~OBJ()
{

}

void OBJ::Load(Ogre::String fileName)
{
	mTriMesh = new btTriangleMesh();

	Ogre::String basename, path;
	Ogre::StringUtil::splitFilename(fileName, basename, path);

	Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource(basename, "General");

	Ogre::String data = pStream->getLine();

	while (!pStream->eof())
	{
		if (true)
		{
			const char* buf = data.c_str();
			char type[32];

			sscanf_s(buf,"%s ", type, 32);
			if (!stricmp(type, "v"))
			{
				float x, y, z;
				sscanf_s(buf, "%*[^ ]%f %f %f", &x, &y, &z);

				mVerts.push_back(btVector3(x, y, z));
			}
			else if (!stricmp(type, "f"))
			{
				int p0,p1,p2;
				sscanf_s(buf, "%*[^ ]%i%*[^ ]%i%*[^ ]%i", &p0, &p1, &p2);
				
				mTriMesh->addTriangle(mVerts[p0-1], mVerts[p1-1], mVerts[p2-1]);
			}
		}

		//Get the next line of the file
		data = pStream->getLine();
	}
}