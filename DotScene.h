//Based on the file here, but some changes were made to make it work with tinyxml2 
//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=DotScene+Loader+with+User+Data

#pragma once

#include <Ogre.h>
#include <vector>
#include <tinyxml2.h>

class nodeProperty
{
public:
   std::string nodeName;
   std::string propertyNm;
   std::string valueName;
 
   nodeProperty(Ogre::String node, Ogre::String propertyName, Ogre::String value)
   {
      nodeName = node;
      propertyNm = propertyName;
      valueName = value;
   }
};

Ogre::Light* LoadLight( tinyxml2::XMLElement *XMLLight, Ogre::SceneManager *mSceneMgr );
 
void parseDotScene( const Ogre::String &SceneName, const Ogre::String& groupName, Ogre::SceneManager *mSceneMgr);

Ogre::String getProperty(Ogre::String ndNm, Ogre::String prop);