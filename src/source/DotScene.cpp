#include "DotScene.h"

std::vector<nodeProperty> nodeProperties;

Ogre::Light* LoadLight( tinyxml2::XMLElement *XMLLight, Ogre::SceneManager *mSceneMgr )
{
	tinyxml2::XMLElement *XMLDiffuse, *XMLSpecular, *XMLAttentuation, *XMLPosition;

   // Create a light (point | directional | spot | radPoint)
   Ogre::Light* l = mSceneMgr->createLight( XMLLight->Attribute("name") );
   if( !XMLLight->Attribute("type") || Ogre::String(XMLLight->Attribute("type")) == "point" )
      l->setType( Ogre::Light::LT_POINT );
   else if( Ogre::String(XMLLight->Attribute("type")) == "directional")
      l->setType( Ogre::Light::LT_DIRECTIONAL );
   else if( Ogre::String(XMLLight->Attribute("type")) == "spot")
      l->setType( Ogre::Light::LT_SPOTLIGHT );
   else if( Ogre::String(XMLLight->Attribute("type")) == "radPoint")
      l->setType( Ogre::Light::LT_POINT );
 
   XMLDiffuse = XMLLight->FirstChildElement("colourDiffuse");
   if( XMLDiffuse ){
      Ogre::ColourValue Diffuse;
      Diffuse.r = Ogre::StringConverter::parseReal( XMLDiffuse->Attribute("r") );
      Diffuse.g = Ogre::StringConverter::parseReal( XMLDiffuse->Attribute("g") );
      Diffuse.b = Ogre::StringConverter::parseReal( XMLDiffuse->Attribute("b") );
      Diffuse.a = 1;
      l->setDiffuseColour(Diffuse);
   }
   XMLSpecular = XMLLight->FirstChildElement("colourSpecular");
   if( XMLSpecular ){
      Ogre::ColourValue Specular;
      Specular.r = Ogre::StringConverter::parseReal( XMLSpecular->Attribute("r") );
      Specular.g = Ogre::StringConverter::parseReal( XMLSpecular->Attribute("g") );
      Specular.b = Ogre::StringConverter::parseReal( XMLSpecular->Attribute("b") );
      Specular.a = 1;
      l->setSpecularColour(Specular);
   }
 
   XMLAttentuation = XMLLight->FirstChildElement("lightAttenuation");
   if( XMLAttentuation )
   {
      //get defaults incase not all values specified
      Ogre::Real range, constant, linear, quadratic;
      range = l->getAttenuationRange();
      constant = l->getAttenuationConstant();
      linear = l->getAttenuationLinear();
      quadratic = l->getAttenuationQuadric();
 
      if( XMLAttentuation->Attribute("range") )
         range = Ogre::StringConverter::parseReal( XMLAttentuation->Attribute("range") );
      if( XMLAttentuation->Attribute("constant") )
         constant = Ogre::StringConverter::parseReal( XMLAttentuation->Attribute("constant") );
      if( XMLAttentuation->Attribute("linear") )
         linear = Ogre::StringConverter::parseReal( XMLAttentuation->Attribute("linear") );
      if( XMLAttentuation->Attribute("quadratic") )
         quadratic = Ogre::StringConverter::parseReal( XMLAttentuation->Attribute("quadratic") );
 
      l->setAttenuation( range, constant, linear, quadratic );
   }
 
   XMLPosition = XMLLight->FirstChildElement("position");
   if( XMLPosition ) {
      Ogre::Vector3 p = Ogre::Vector3(0,0,0);
      if( XMLPosition->Attribute("x") )
         p.x = Ogre::StringConverter::parseReal( XMLPosition->Attribute("x") );
      if( XMLPosition->Attribute("y") )
         p.y = Ogre::StringConverter::parseReal( XMLPosition->Attribute("y") );
      if( XMLPosition->Attribute("z") )
         p.z = Ogre::StringConverter::parseReal( XMLPosition->Attribute("z") );
 
      l->setPosition( p );
   }
 
   //castShadows      (true | false) "true"
   l->setCastShadows( true );
   if( XMLLight->Attribute("castShadows") )
      if( Ogre::String(XMLLight->Attribute("castShadows")) == "false" )
         l->setCastShadows( false );
 
   //visible         (true | false) "true"      
   l->setVisible( true );
   if( XMLLight->Attribute("visible") )
      if( Ogre::String(XMLLight->Attribute("visible")) == "false" )
         l->setVisible( false );
 
   return l;
}

bool parseDotScene( const Ogre::String &SceneName, const Ogre::String& groupName, Ogre::SceneManager *mSceneMgr )
{
	tinyxml2::XMLDocument   *XMLDoc;
   tinyxml2::XMLElement   *XMLRoot, *XMLNodes;
 
   try
   {
      Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().
         openResource( SceneName, groupName );
 
      Ogre::String data = pStream->getAsString();
      // Open the .scene File
	  XMLDoc = new tinyxml2::XMLDocument();
      XMLDoc->Parse( data.c_str() );
      pStream->close();
      pStream.setNull();
 
      if( XMLDoc->Error() )
      {
         //We'll just log, and continue on gracefully
         Ogre::LogManager::getSingleton().logMessage("[dotSceneLoader] The TiXmlDocument reported an error");
         delete XMLDoc;

		 //Return false to say that there was an error and that we should try to use a backup method for creating a scene.
         return false;
      }
   }
   catch(...)
   {
      //We'll just log, and continue on gracefully
      Ogre::LogManager::getSingleton().logMessage("[dotSceneLoader] Error creating TiXmlDocument");
      delete XMLDoc;

	  //Return false to say that there was an error and that we should try to use a backup method for creating a scene.
      return false;
   }
 
   // Validate the File
   XMLRoot = XMLDoc->RootElement();
   if( Ogre::String( XMLRoot->Value()) != "scene"  ) {
      Ogre::LogManager::getSingleton().logMessage( "[dotSceneLoader]Error: Invalid .scene File. Missing <scene>" );
      delete XMLDoc;  

	  //Return false to say that there was an error and that we should try to use a backup method for creating a scene.
      return false;
   }
 
   XMLNodes = XMLRoot->FirstChildElement( "nodes" );
 
   // Read in the scene nodes
   if( XMLNodes )
   {
	   tinyxml2::XMLElement *XMLNode, *XMLPosition, *XMLRotation, *XMLScale,  *XMLEntity, *XMLBillboardSet,  *XMLLight, *XMLUserData;
 
      XMLNode = XMLNodes->FirstChildElement( "node" );
 
      while( XMLNode )
      {
         // Process the current node
         // Grab the name of the node
         Ogre::String NodeName = XMLNode->Attribute("name");
         // First create the new scene node
         Ogre::SceneNode* NewNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( NodeName );
         Ogre::Vector3 TempVec;
         Ogre::String TempValue;
 
         // Now position it...
         XMLPosition = XMLNode->FirstChildElement("position");
         if( XMLPosition ){
            TempValue = XMLPosition->Attribute("x");
            TempVec.x = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLPosition->Attribute("y");
            TempVec.y = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLPosition->Attribute("z");
            TempVec.z = Ogre::StringConverter::parseReal(TempValue);
            NewNode->setPosition( TempVec );
         }
 
         // Rotate it...
         XMLRotation = XMLNode->FirstChildElement("rotation");
         if( XMLRotation ){
            Ogre::Quaternion TempQuat;
            TempValue = XMLRotation->Attribute("qx");
            TempQuat.x = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLRotation->Attribute("qy");
            TempQuat.y = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLRotation->Attribute("qz");
            TempQuat.z = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLRotation->Attribute("qw");
            TempQuat.w = Ogre::StringConverter::parseReal(TempValue);
            NewNode->setOrientation( TempQuat );
         }
 
         // Scale it.
         XMLScale = XMLNode->FirstChildElement("scale");
         if( XMLScale ){
            TempValue = XMLScale->Attribute("x");
            TempVec.x = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLScale->Attribute("y");
            TempVec.y = Ogre::StringConverter::parseReal(TempValue);
            TempValue = XMLScale->Attribute("z");
            TempVec.z = Ogre::StringConverter::parseReal(TempValue);
            NewNode->setScale( TempVec );
         }
 
         XMLLight = XMLNode->FirstChildElement( "light" );
         if( XMLLight )
            NewNode->attachObject( LoadLight( XMLLight, mSceneMgr ) );
 
         // Check for an Entity
         XMLEntity = XMLNode->FirstChildElement("entity");
         if( XMLEntity )
         {
            Ogre::String EntityName, EntityMeshFilename;
            EntityName = XMLEntity->Attribute( "name" );
            EntityMeshFilename = XMLEntity->Attribute( "meshFile" );
 
            // Create entity
            Ogre::Entity* NewEntity = mSceneMgr->createEntity(EntityName, EntityMeshFilename);
 
            //castShadows      (true | false) "true"
        NewEntity->setCastShadows( true );
        if( XMLEntity->Attribute("castShadows") )
               if( Ogre::String(XMLEntity->Attribute("castShadows")) == "false" )
                  NewEntity->setCastShadows( false );
 
            NewNode->attachObject( NewEntity );
         }
 
         XMLBillboardSet = XMLNode->FirstChildElement( "billboardSet" );
         if( XMLBillboardSet )
         {
            Ogre::String TempValue;
 
            Ogre::BillboardSet* bSet = mSceneMgr->createBillboardSet( NewNode->getName() );
 
            Ogre::BillboardType Type;
            TempValue = XMLBillboardSet->Attribute( "type" );
            if( TempValue == "orientedCommon" )
               Type = Ogre::BBT_ORIENTED_COMMON;
            else if( TempValue == "orientedSelf" )
               Type = Ogre::BBT_ORIENTED_SELF;
            else Type = Ogre::BBT_POINT;
 
            Ogre::BillboardOrigin Origin;
            TempValue = XMLBillboardSet->Attribute( "type" );
            if( TempValue == "bottom_left" )
               Origin = Ogre::BBO_BOTTOM_LEFT;
            else if( TempValue == "bottom_center" )
               Origin = Ogre::BBO_BOTTOM_CENTER;
            else if( TempValue == "bottomRight"  )
               Origin = Ogre::BBO_BOTTOM_RIGHT;
            else if( TempValue == "left" )
               Origin = Ogre::BBO_CENTER_LEFT;
            else if( TempValue == "right" )
               Origin = Ogre::BBO_CENTER_RIGHT;
            else if( TempValue == "topLeft" )
               Origin = Ogre::BBO_TOP_LEFT;
            else if( TempValue == "topCenter" )
               Origin = Ogre::BBO_TOP_CENTER;
            else if( TempValue == "topRight" )
               Origin = Ogre::BBO_TOP_RIGHT;
            else
               Origin = Ogre::BBO_CENTER;
 
            bSet->setBillboardType( Type );
            bSet->setBillboardOrigin( Origin );
 
            TempValue = XMLBillboardSet->Attribute( "name" );
            bSet->setMaterialName( TempValue );
 
            int width, height;
            width = (int) Ogre::StringConverter::parseReal( XMLBillboardSet->Attribute( "width" ) );
            height = (int) Ogre::StringConverter::parseReal( XMLBillboardSet->Attribute( "height" ) );
            bSet->setDefaultDimensions( width, height );
            bSet->setVisible( true );
            NewNode->attachObject( bSet );
 
			tinyxml2::XMLElement *XMLBillboard;
 
            XMLBillboard = XMLBillboardSet->FirstChildElement( "billboard" );
 
            while( XMLBillboard )
            {
               Ogre::Billboard *b;
               // TempValue;
               TempVec = Ogre::Vector3( 0, 0, 0 );
               Ogre::ColourValue TempColour(1,1,1,1);
 
               XMLPosition = XMLBillboard->FirstChildElement( "position" );
               if( XMLPosition ){
                  TempValue = XMLPosition->Attribute("x");
                  TempVec.x = Ogre::StringConverter::parseReal(TempValue);
                  TempValue = XMLPosition->Attribute("y");
                  TempVec.y = Ogre::StringConverter::parseReal(TempValue);
                  TempValue = XMLPosition->Attribute("z");
                  TempVec.z = Ogre::StringConverter::parseReal(TempValue);
               }
 
			   tinyxml2::XMLElement* XMLColour = XMLBillboard->FirstChildElement( "colourDiffuse" );
               if( XMLColour ){
                  TempValue = XMLColour->Attribute("r");
                  TempColour.r = Ogre::StringConverter::parseReal(TempValue);
                  TempValue = XMLColour->Attribute("g");
                  TempColour.g = Ogre::StringConverter::parseReal(TempValue);
                  TempValue = XMLColour->Attribute("b");
                  TempColour.b = Ogre::StringConverter::parseReal(TempValue);
               }
 
               b = bSet->createBillboard( TempVec, TempColour);
 
               XMLBillboard = XMLBillboard->NextSiblingElement( "billboard" );
            }
         }
 
         XMLUserData = XMLNode->FirstChildElement( "userData" );
         if ( XMLUserData )
         {
			 tinyxml2::XMLElement *XMLProperty;
            XMLProperty = XMLUserData->FirstChildElement("property");
            while ( XMLProperty )
            {
               Ogre::String first = NewNode->getName();
               Ogre::String second = XMLProperty->Attribute("name");
               Ogre::String third = XMLProperty->Attribute("data");
               nodeProperty newProp(first,second,third);
               nodeProperties.push_back(newProp);
               XMLProperty = XMLProperty->NextSiblingElement("property");
            }   
         }
 
         // Move to the next node
         XMLNode = XMLNode->NextSiblingElement( "node" );
      }
   }
 
   // Close the XML File
   delete XMLDoc;

   //Return with true to say that the file was loaded sucsessfully and the scene is now loaded.
   return true;
}

Ogre::String getProperty(Ogre::String ndNm, Ogre::String prop)
{
	bool could = false;
   for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ )
   {
      if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop )
      {
         return nodeProperties[i].valueName;
         could = true;
      }
   }
   if (could == false)
      return " ";
}