/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/


#include <iostream>


#include <sofa/core/SofaLibrary.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/Base.h>

#include <sofa/helper/vector.h>


#include <sofa/helper/system/SetDirectory.h>
#include <sofa/helper/system/FileRepository.h>

#include <sofa/component/init.h>
#include <algorithm>
#include <functional>
using sofa::core::SofaLibrary;
using sofa::core::CategoryLibrary;
using sofa::core::ComponentLibrary;


const std::string headerFile(
    "\
/******************************************************************************\n\
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *\n\
*                (c) 2006-2010 MGH, INRIA, USTL, UJF, CNRS                    *\n\
*                                                                             *\n\
* This library is free software; you can redistribute it and/or modify it     *\n\
* under the terms of the GNU Lesser General Public License as published by    *\n\
* the Free Software Foundation; either version 2.1 of the License, or (at     *\n\
* your option) any later version.                                             *\n\
*                                                                             *\n\
* This library is distributed in the hope that it will be useful, but WITHOUT *\n\
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *\n\
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *\n\
* for more details.                                                           *\n\
*                                                                             *\n\
* You should have received a copy of the GNU Lesser General Public License    *\n\
* along with this library; if not, write to the Free Software Foundation,     *\n\
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *\n\
*******************************************************************************\n\
*                               SOFA :: Modules                               *\n\
*                                                                             *\n\
* Authors: The SOFA Team and external contributors (see Authors.txt)          *\n\
*                                                                             *\n\
* Contact information: contact@sofa-framework.org                             *\n\
******************************************************************************/\n\
\n\n\n\
// File automatically generated by \"generateTypedef\"\n\n\
"
);

enum TYPES {TYPE_DOUBLE, TYPE_FLOAT, TYPE_COMBINATION};

typedef sofa::core::ObjectFactory::Creator Creator;

static std::map< std::string, std::string >               templateExtension;
static std::map< TYPES, std::string >                     fileExtension;


static std::map< std::string, std::string >               includeComponents;
static std::multimap< std::string, std::string >          typedefComponents;
static std::multimap< std::string, std::string >          simplificationTypedefComponents;
static std::multimap< std::string, std::string >          bannedComponents;

static const std::string multimappingName  = "MultiMapping";
static const std::string multi2mappingName = "Multi2Mapping";
static const std::string mechanicalmultimappingName  = "MechanicalMultiMapping";
static const std::string mechanicalmulti2mappingName = "MechanicalMulti2Mapping";


struct removeLastChar
{
    void operator()( std::string& str )
    {
        str = str.substr(0,str.size() - 1);
    }
};


struct applyTemplateExtension
{
    std::string operator()(std::string& str )
    {
        return templateExtension[str];
    }
};

struct rule
{
    bool result;
    rule():result(true) {};
};

struct onlyDouble : public rule
{
    void operator()(std::string& templateParam )
    {
        if ( *templateParam.rbegin() == 'd' ) this->result = ( true && this->result );
        else this->result = false;
    }
};

struct onlyFloat : public rule
{
    void operator()(std::string& templateParam )
    {
        if( *templateParam.rbegin() == 'f' ) this->result = ( true && this->result );
        else this->result = false;
    }
};

struct onlyExtVec3f : public rule
{
    void operator()(std::string& templateParam )
    {
        if ( templateParam == "ExtVec3f" )  this->result = this->result && true;
        else this->result = false;
    }
};

struct onlyExtVec3d : public rule
{
    void operator()(std::string& templateParam )
    {
        if ( templateParam == "ExtVec3d" )  this->result = this->result && true;
        else this->result = false;
    }
};

struct validTemplate : public rule
{
    void operator()(std::string& templateParam )
    {
        if( templateExtension.find(templateParam) !=  templateExtension.end() ) this->result = this->result && true;
        else this->result = false;
    }
};

template< typename Rule >
bool applyRule( sofa::helper::vector<std::string>& templateList)
{
    Rule r;
    r = std::for_each( templateList.begin(), templateList.end(), r );
    return r.result;
}

bool belongToBannedComponents(const std::string& category, const std::string& component )
{
    std::multimap< std::string, std::string>::iterator it;
    std::multimap< std::string, std::string>::iterator end;

    it = bannedComponents.find(category);
    if ( it == bannedComponents.end() ) return false;

    end = bannedComponents.upper_bound(category);

    for ( ; it != end; ++it )
    {
        if ( component == it->second) return true;
    }

    return false;
}

void pushToList( const std::string& templateCombination, const char separator, sofa::helper::vector< std::string>& templateList )
{
    size_t curPos = 0;
    size_t oldPos = 0;
    std::string chunk;
    while ( ( curPos = templateCombination.find(separator, oldPos) ) != std::string::npos )
    {
        templateList.push_back( templateCombination.substr(oldPos,curPos) );
        oldPos = curPos+1;
    }
    templateList.push_back( templateCombination.substr(oldPos) );
}


void parseTemplateCombination(const std::string& templateCombination,
        sofa::helper::vector<std::string>& inputTemplateList,
        sofa::helper::vector<std::string>& outputTemplateList )
{
    size_t o_bracketPos = 0;
    size_t c_bracketPos = 0;
    size_t separatorPos = 0;

    o_bracketPos = templateCombination.find('[');
    // case one inputTemplateParam and one outputTemplateParam
    if ( o_bracketPos == std::string::npos )
    {
        separatorPos = templateCombination.find(',');
        assert( separatorPos != std::string::npos);
        inputTemplateList.push_back( templateCombination.substr(0,separatorPos) );
        outputTemplateList.push_back( templateCombination.substr(separatorPos+1) );
    }
    else
    {
        if ( o_bracketPos == 0 )
        {
            c_bracketPos = templateCombination.find(']');
            assert( c_bracketPos != std::string::npos && c_bracketPos > o_bracketPos );
            pushToList( templateCombination.substr(o_bracketPos+1,c_bracketPos-1),',',inputTemplateList );
            o_bracketPos = templateCombination.find( '[' , c_bracketPos+1 );
            if ( o_bracketPos == std::string::npos )
            {
                // single output template param
                assert( templateCombination[ c_bracketPos + 1] == ',' );
                outputTemplateList.push_back( templateCombination.substr(c_bracketPos + 2 ) );
            }
            else
            {
                // many output template params.
                c_bracketPos = templateCombination.find(']');
                assert( c_bracketPos != std::string::npos && c_bracketPos > o_bracketPos );
                pushToList( templateCombination.substr(o_bracketPos+1,c_bracketPos-1),',',outputTemplateList );

            }
        }
        else
        {
            // one input template param, many output template params.
            assert ( templateCombination[ o_bracketPos - 1] == ',' );
            inputTemplateList.push_back( templateCombination.substr(0, o_bracketPos - 1 ) );
            c_bracketPos = templateCombination.find(']');
            assert( c_bracketPos != std::string::npos && c_bracketPos > o_bracketPos );
            pushToList( templateCombination.substr(o_bracketPos+1, c_bracketPos-1),',',outputTemplateList );
        }

        /*std::cout << "input list:" << std::endl;
          std::copy( inputTemplateList.begin(), inputTemplateList.end(), std::ostream_iterator<std::string>( std::cout, " " ));
          std::cout << std::endl;
          std::cout << "output list:" << std::endl;
          std::copy( outputTemplateList.begin(), outputTemplateList.end(), std::ostream_iterator<std::string>( std::cout, " " ));
          std::cout << std::endl;
          */
    }
}

std::string cat(sofa::helper::vector<std::string>& first, sofa::helper::vector<std::string>& second, std::string& separator)
{
    std::ostringstream oss;
    assert ( first.size() != 0 && second.size() != 0 );
    if (!first.empty())
    {
        if( first.size() > 1 )
        {
            std::copy( first.begin(), first.end() - 1, std::ostream_iterator<std::string>( oss, "_" ) );
            oss << first.back();
        }
        else oss << first[0];
        oss << separator;
    }
    if (!second.empty())
    {
        if( second.size() > 1 )
        {
            std::copy( second.begin(), second.end() - 1, std::ostream_iterator<std::string>( oss , "_" ) );
            oss << second.back();
        }
        else oss << second[0];
    }
    oss.flush();
    return oss.str();
}


void printIncludes( const CategoryLibrary &category)
{

    const std::string pathIncludeFiles = sofa::helper::system::SetDirectory::GetParentDir(sofa::helper::system::DataRepository.getFirstPath().c_str()) + "/modules/";


    const CategoryLibrary::VecComponent &components = category.getComponents();
    for (CategoryLibrary::VecComponentIterator itComp=components.begin(); itComp != components.end(); ++itComp)
    {
        std::ostringstream output;

        const ComponentLibrary &component = *(*itComp);

        if ( belongToBannedComponents(category.getName(), component.getName() ) )continue;

        const std::type_info& defaultTypeInfo=component.getEntry()->creatorList.begin()->second->type();

        std::string namespaceComponent = sofa::core::objectmodel::Base::decodeNamespaceName(component.getEntry()->creatorList.begin()->second->type());

        std::size_t positionDoublePoints = namespaceComponent.find("::");
        while (positionDoublePoints != std::string::npos)
        {
            namespaceComponent.replace(positionDoublePoints, 2, "/");
            positionDoublePoints =  namespaceComponent.find("::");
        }

        std::string filename = namespaceComponent+"/"+sofa::core::objectmodel::Base::decodeClassName(defaultTypeInfo)+".h";

        std::string f(pathIncludeFiles+filename);
        if (sofa::helper::system::DataRepository.findFile( f ) )
        {
            output << "#include <" << namespaceComponent  << "/"
                    << sofa::core::objectmodel::Base::decodeClassName(defaultTypeInfo) << ".h>\n";
            includeComponents.insert(std::make_pair(component.getName(),output.str()));
        }

    }
};


void printFullTypedefs( const CategoryLibrary &category, TYPES t)
{

    //   output << "\n\n//Declaration of the typedefs \n";
    sofa::helper::vector< std::pair< std::string, std::string > > typedefWritten;
    const CategoryLibrary::VecComponent &components = category.getComponents();
    for (CategoryLibrary::VecComponentIterator itComp=components.begin(); itComp != components.end(); ++itComp)
    {
        const ComponentLibrary &component = *(*itComp);
        const std::list< std::pair<std::string, Creator*> > &creatorList=component.getEntry()->creatorList;
        std::list< std::pair<std::string, Creator*> >::const_iterator itCreator;

        if ( belongToBannedComponents(category.getName(), component.getName() ) ) continue;

        //Iterate on all the possible creation of templates
        for (itCreator=creatorList.begin(); itCreator!=creatorList.end(); ++itCreator)
        {
            const std::type_info& typeInfo=itCreator->second->type();

            std::string templateName = itCreator->first;

            if (templateExtension.find(templateName) != templateExtension.end())
            {
                std::string extension=templateExtension[templateName];
                bool isFloat = ( *(extension.rbegin()) == 'f');
                if ( (isFloat && t==TYPE_FLOAT) || (!isFloat && t==TYPE_DOUBLE))
                {
                    const std::string finalName = component.getName() +  templateExtension[templateName];

                    simplificationTypedefComponents.insert(std::make_pair(component.getName(),finalName + " " + finalName.substr(0,finalName.size()-1)));
                    typedefComponents.insert(std::make_pair(component.getName(), sofa::core::objectmodel::Base::decodeFullName(typeInfo) + " " + finalName));
                }
            }
            else
            {
                //Template not recognized

                std::string categoryName=category.getName();
                std::string componentName=component.getName();
                std::string templateCombination=templateName;
                bool isMapping=false;

                //Special case for Mapping and MultiMappings !
                if ( templateName.substr(0,multimappingName.size()) == multimappingName )
                {
                    categoryName = multimappingName;
                }
                if ( templateName.substr(0,multi2mappingName.size()) == multi2mappingName )
                {
                    categoryName = multi2mappingName;
                }
                if ( templateName.substr(0,mechanicalmultimappingName.size()) == mechanicalmultimappingName )
                {
                    categoryName = mechanicalmultimappingName;
                }
                if ( templateName.substr(0,mechanicalmulti2mappingName.size()) == mechanicalmulti2mappingName )
                {
                    categoryName = mechanicalmulti2mappingName;
                }
                if (templateName.substr(0, categoryName.size()) == categoryName )
                {
                    //Special case of Mappings
                    //Transform: (Mechanical)(Multi)Mapping<Vec3d,Vec3d> into Vec3d,Vec3d
                    templateCombination = templateName.substr(categoryName.size()+1);
                    templateCombination = templateCombination.substr(0,templateCombination.size()-1);
                    isMapping=true;
                    //Change the name of the typedef to do a difference between a mapping and a mechanical mapping
                    if (categoryName.find("MechanicalMapping") != std::string::npos)
                    {
                        componentName = componentName.substr(0,componentName.size()-7);
                        componentName += "MechanicalMapping";
                    }
                    if( categoryName.find(mechanicalmultimappingName) != std::string::npos )
                    {
                        componentName = componentName.substr(0,componentName.size()-multimappingName.size() );
                        componentName += mechanicalmultimappingName;
                    }
                    if( categoryName.find(mechanicalmulti2mappingName) != std::string::npos )
                    {
                        componentName = componentName.substr(0,componentName.size()-multi2mappingName.size() );
                        componentName += mechanicalmulti2mappingName;
                    }
                }

                sofa::helper::vector<std::string> inputTemplateList;
                sofa::helper::vector<std::string> outputTemplateList;
                if ( templateCombination.find(',' ) != std::string::npos )
                {
                    parseTemplateCombination(templateCombination,inputTemplateList,outputTemplateList);
                }
                if ( applyRule<validTemplate>(inputTemplateList) && applyRule<validTemplate>(outputTemplateList) )
                {
                    bool inputFloat   = applyRule<onlyFloat>(inputTemplateList);
                    bool inputDouble  = applyRule<onlyDouble>(inputTemplateList);
                    bool outputFloat  = applyRule<onlyFloat>(outputTemplateList);
                    bool outputDouble = applyRule<onlyDouble>(outputTemplateList);
                    switch( t )
                    {
                    case TYPE_FLOAT:
                        if( inputDouble || outputDouble ) continue;
                        break;
                    case TYPE_DOUBLE:
                        if( inputFloat ) continue;
                        if( outputFloat && !applyRule<onlyExtVec3f>(outputTemplateList) ) continue;
                        break;
                    case TYPE_COMBINATION:
                        if( inputDouble && outputDouble ) continue;
                        if( inputFloat  && outputFloat  ) continue;
                        if( applyRule<onlyExtVec3f>(outputTemplateList) ) continue;
                        break;
                    }
                    std::string separatorCombination;
                    if (isMapping) separatorCombination="_to_";
                    else           separatorCombination="_";

                    sofa::helper::vector<std::string> inputTemplateExtensions;
                    inputTemplateExtensions.resize(inputTemplateList.size() );
                    sofa::helper::vector<std::string> outputTemplateExtensions;
                    outputTemplateExtensions.resize(outputTemplateList.size() );
                    applyTemplateExtension applyTemplateExtensionFn;
                    std::transform(inputTemplateList.begin(), inputTemplateList.end(), inputTemplateExtensions.begin(), applyTemplateExtensionFn );
                    std::transform(outputTemplateList.begin(), outputTemplateList.end(), outputTemplateExtensions.begin(), applyTemplateExtensionFn );
                    const std::string finalName = componentName + cat(inputTemplateExtensions,outputTemplateExtensions,separatorCombination);
                    std::string smartFinalName;

                    removeLastChar removeFn;
                    if ( !applyRule<onlyExtVec3d>(outputTemplateList) )
                    {
                        std::for_each(inputTemplateExtensions.begin(), inputTemplateExtensions.end(), removeFn  );
                        std::for_each(outputTemplateExtensions.begin(), outputTemplateExtensions.end(), removeFn );
                        smartFinalName = componentName + cat(inputTemplateExtensions,outputTemplateExtensions,separatorCombination);
                    }
                    else
                    {
                        std::for_each(inputTemplateExtensions.begin(), inputTemplateExtensions.end(), removeFn );
                        smartFinalName = componentName + cat(inputTemplateExtensions,outputTemplateExtensions,separatorCombination);;
                    }
                    simplificationTypedefComponents.insert(std::make_pair(component.getName(),finalName + " " + smartFinalName));
                    typedefComponents.insert(std::make_pair(component.getName(), sofa::core::objectmodel::Base::decodeFullName(typeInfo) + " " + finalName));
                }
            }
        }
    }
}

static std::string outputPath=sofa::helper::system::SetDirectory::GetParentDir(sofa::helper::system::DataRepository.getFirstPath().c_str()) + "/modules/sofa/component/typedef/";

void writeFile(const CategoryLibrary &category,  TYPES t, std::ostream &generalOutput)
{

    if (includeComponents.empty() || typedefComponents.empty()) return;


    const std::string filename=category.getName()+fileExtension[t] + ".h";

    generalOutput << "#include <sofa/component/typedef/" << filename << ">\n";

    std::ofstream typedefFile( (outputPath+filename).c_str() );
    typedefFile << headerFile << "\n";
    typedefFile << "#ifndef SOFA_TYPEDEF_" << category.getName() << fileExtension[t] << "_H" << "\n"
            << "#define SOFA_TYPEDEF_" << category.getName() << fileExtension[t] << "_H" << "\n";


    //---------------------------------------------------------------------------------------------
    //DEFAULT INCLUDES
    typedefFile <<
            "\n\
//Default files containing the declaration of the vector type\n\
#include <sofa/defaulttype/VecTypes.h>\n\
#include <sofa/defaulttype/RigidTypes.h>\n\
#include <sofa/defaulttype/Mat.h>\n\n\
\n\
#ifdef SOFA_GPU_CUDA\n\
#include <sofa/gpu/cuda/CudaTypesBase.h>\n\
#include <sofa/gpu/cuda/CudaTypes.h>\n\
#endif\n\
#ifdef SOFA_GPU_OPENCL\n\
#include <sofa/gpu/opencl/OpenCLTypes.h>\n\
#endif\n\
";

            if (category.getName() == "Mapping")
            {
            typedefFile << "\
            //Default files needed to create a Mapping\n\
#include <sofa/core/behavior/State.h>\n\
#include <sofa/core/behavior/MappedModel.h>\n\
#include <sofa/core/Mapping.h>\n\
";
}
        else if (category.getName() == "MechanicalMapping")
{
    typedefFile << "\
//Default files needed to create a Mechanical Mapping\n\
#include <sofa/core/behavior/MechanicalState.h>\n\
#include <sofa/core/behavior/MechanicalMapping.h>\n\
";
}
    typedefFile << "\n\n";

    //---------------------------------------------------------------------------------------------
    //COMPONENTS INCLUDES
    std::map< std::string, std::string >::const_iterator itIncludes;
    for (itIncludes=includeComponents.begin(); itIncludes!=includeComponents.end(); ++itIncludes)
{
    //Only write the include if a typedef will follow
    if (typedefComponents.count(itIncludes->first))
{
    typedefFile << itIncludes->second;
}
}

    typedefFile << "\n\n";

    typedef std::multimap< std::string, std::string >::iterator multimapConstIterator;
    //---------------------------------------------------------------------------------------------
    //TYPEDEF DECLARATIONS
    for (itIncludes=includeComponents.begin(); itIncludes!=includeComponents.end(); ++itIncludes)
{
    std::pair<multimapConstIterator,multimapConstIterator > range;
    range = typedefComponents.equal_range(itIncludes->first);

    if (typedefComponents.count(itIncludes->first))
{
    typedefFile << "\n//---------------------------------------------------------------------------------------------\n\
    //Typedef for " << itIncludes->first << "\n";
    for (multimapConstIterator it=range.first; it!=range.second; it++)
    {
        std::string& str = (it->second);
#ifdef WIN32
        static const std::string keyword_class = "class";
        size_t pos  = 0;
        while( ( pos = str.find(keyword_class,pos) ) != std::string::npos )
        {
            str.replace(pos, keyword_class.length() ,"");
            pos += 1;
        }
#endif
        typedefFile << "typedef " << str << ";\n";
    }
    typedefFile << "\n\n";
}
}


typedefFile << "\n\n";
//---------------------------------------------------------------------------------------------
//TYPEDEF SIMPLIFICATIONS
if (t != TYPE_COMBINATION)
{
    switch (t)
    {
    case TYPE_DOUBLE:
    {
        typedefFile << "\n#ifndef SOFA_FLOAT\n";
        break;
    }
    case TYPE_FLOAT:
    {
        typedefFile << "\n#ifdef SOFA_FLOAT\n";
        break;
    }
    default:
    {
        break;
    }
    }

    for (itIncludes=includeComponents.begin(); itIncludes!=includeComponents.end(); ++itIncludes)
    {
        std::pair<multimapConstIterator,multimapConstIterator > range;
        range = simplificationTypedefComponents.equal_range(itIncludes->first);

        for (multimapConstIterator it=range.first; it!=range.second; it++)
        {
            typedefFile << "typedef " << it->second << ";\n";
        }
    }

    typedefFile << "#endif\n";
}
//---------------------------------------------------------------------------------------------
//CLOSE THE FILE
typedefFile << "\n#endif\n";
typedefFile.close();
}

int main(int , char** )
{
#ifdef WIN32
    bannedComponents.insert(std::pair<std::string,std::string>("Controller","ComplianceMatrixUpdateManager" ) );
    bannedComponents.insert(std::pair<std::string,std::string>("Controller","ComplianceMatrixUpdateManagerCarving" ) );
    //bannedComponents.insert(std::pair<std::string,std::string>("Controller","HandStateController" ) );
    //bannedComponents.insert(std::pair<std::string,std::string>("Controller","JointSpringController") );
    bannedComponents.insert(std::pair<std::string,std::string>("Controller","TetrahedronCuttingManager") );
    bannedComponents.insert(std::pair<std::string,std::string>("VisualModel","FlowVisualModel") );
#endif

    sofa::component::init();
    SofaLibrary library; library.build();
    const SofaLibrary::VecCategory &categories = library.getCategories();

    templateExtension.insert(std::make_pair("Vec6d", "6d"));
    templateExtension.insert(std::make_pair("Vec4d", "4d"));
    templateExtension.insert(std::make_pair("Vec3d", "3d"));
    templateExtension.insert(std::make_pair("Vec2d", "2d"));
    templateExtension.insert(std::make_pair("Vec1d", "1d"));
    templateExtension.insert(std::make_pair("Rigid", "Rigid3d"));
    templateExtension.insert(std::make_pair("Rigid2d", "Rigid2d"));

    templateExtension.insert(std::make_pair("Vec6f", "6f"));
    templateExtension.insert(std::make_pair("Vec4f", "4f"));
    templateExtension.insert(std::make_pair("Vec3f", "3f"));
    templateExtension.insert(std::make_pair("Vec2f", "2f"));
    templateExtension.insert(std::make_pair("Vec1f", "1f"));
    templateExtension.insert(std::make_pair("Rigid3f", "Rigid3f"));
    templateExtension.insert(std::make_pair("Rigid2f", "Rigid2f"));
    templateExtension.insert(std::make_pair("ExtVec3f", "Ext3f"));
    templateExtension.insert(std::make_pair("ExtVec3d", "Ext3d"));

#ifdef SOFA_GPU_CUDA
    templateExtension.insert(std::make_pair("CudaVec6d", "Cuda6d"));
    templateExtension.insert(std::make_pair("CudaVec4d", "Cuda4d"));
    templateExtension.insert(std::make_pair("CudaVec3d", "Cuda3d"));
    templateExtension.insert(std::make_pair("CudaVec2d", "Cuda2d"));
    templateExtension.insert(std::make_pair("CudaVec1d", "Cuda1d"));
    templateExtension.insert(std::make_pair("CudaRigid3d", "CudaRigid3d"));
    templateExtension.insert(std::make_pair("CudaRigid2d", "CudaRigid2d"));

    templateExtension.insert(std::make_pair("CudaVec6f", "Cuda6f"));
    templateExtension.insert(std::make_pair("CudaVec4f", "Cuda4f"));
    templateExtension.insert(std::make_pair("CudaVec3f", "Cuda3f"));
    templateExtension.insert(std::make_pair("CudaVec2f", "Cuda2f"));
    templateExtension.insert(std::make_pair("CudaVec1f", "Cuda1f"));
    templateExtension.insert(std::make_pair("CudaRigid3f", "CudaRigid3f"));
    templateExtension.insert(std::make_pair("CudaRigid2f", "CudaRigid2f"));
#endif

#ifdef SOFA_GPU_OPENCL
    templateExtension.insert(std::make_pair("OpenCLVec6d", "OpenCL6d"));
    templateExtension.insert(std::make_pair("OpenCLVec4d", "OpenCL4d"));
    templateExtension.insert(std::make_pair("OpenCLVec3d", "OpenCL3d"));
    templateExtension.insert(std::make_pair("OpenCLVec2d", "OpenCL2d"));
    templateExtension.insert(std::make_pair("OpenCLVec1d", "OpenCL1d"));
    templateExtension.insert(std::make_pair("OpenCLRigid3d", "OpenCLRigid3d"));
    templateExtension.insert(std::make_pair("OpenCLRigid2d", "OpenCLRigid2d"));

    templateExtension.insert(std::make_pair("OpenCLVec6f", "OpenCL6f"));
    templateExtension.insert(std::make_pair("OpenCLVec4f", "OpenCL4f"));
    templateExtension.insert(std::make_pair("OpenCLVec3f", "OpenCL3f"));
    templateExtension.insert(std::make_pair("OpenCLVec2f", "OpenCL2f"));
    templateExtension.insert(std::make_pair("OpenCLVec1f", "OpenCL1f"));
    templateExtension.insert(std::make_pair("OpenCLRigid3f", "OpenCLRigid3f"));
    templateExtension.insert(std::make_pair("OpenCLRigid2f", "OpenCLRigid2f"));
#endif

    fileExtension.insert    (std::make_pair(TYPE_DOUBLE, "_double"));
    fileExtension.insert    (std::make_pair(TYPE_FLOAT, "_float"));
    fileExtension.insert    (std::make_pair(TYPE_COMBINATION, "_combination"));


    std::string filenameSofaTypedef=outputPath+"Sofa_typedef.h";
    std::ofstream sofaTypedef( filenameSofaTypedef.c_str() ); sofaTypedef << headerFile << "\n";
    sofaTypedef << "\
#ifndef SOFA_TYPEDEF\n\
#define SOFA_TYPEDEF\n\
\n\n\
//Compilation WITHOUT double\n\
#ifndef SOFA_DOUBLE\n\
#include <sofa/component/typedef/Sofa_float.h>\n\
#endif\n\
\n\
//Compilation WITHOUT float\n\
#ifndef SOFA_FLOAT\n\
#include <sofa/component/typedef/Sofa_double.h>\n\
#endif\n\
\n\n\
#ifndef SOFA_DOUBLE\n\
#ifndef SOFA_FLOAT\n\
\n";

    std::string filenameSofaDouble=outputPath+"Sofa_double.h";
    std::ofstream sofaDouble( filenameSofaDouble.c_str() );   sofaDouble  << headerFile << "\n";
    sofaDouble << "\
#ifndef SOFA_TYPEDEF_DOUBLE\n\
#define SOFA_TYPEDEF_DOUBLE\n\
\n";
    sofaDouble << "\n#include <sofa/component/typedef/Particles_double.h>\n";

    std::string filenameSofaFloat=outputPath+"Sofa_float.h";
    std::ofstream sofaFloat( filenameSofaFloat.c_str() );     sofaFloat   << headerFile << "\n";
    sofaFloat << "\
#ifndef SOFA_TYPEDEF_FLOAT\n\
#define SOFA_TYPEDEF_FLOAT\n\
\n";
    sofaFloat << "\n#include <sofa/component/typedef/Particles_float.h>\n";

std::cout << "Generating Type definition for Sofa Component: " << std::endl;
    for (SofaLibrary::VecCategoryIterator itCat=categories.begin(); itCat!=categories.end(); ++itCat)
    {
    const CategoryLibrary &category = *(*itCat);
    std::cerr << "\tProcessing " << category.getNumComponents() << " components for the " << category.getName();
        const CategoryLibrary::VecComponent &components = category.getComponents();
        //First read all the components of the categories, and try to know if templates exist
        bool needToCreateTypedefs=false;
        for (CategoryLibrary::VecComponentIterator itComp=components.begin(); itComp != components.end(); ++itComp)
    {
        const ComponentLibrary &component = *(*itComp);
        if (!component.getTemplates().empty()) {needToCreateTypedefs=true; break;}
    }

        //If templates have been found, we create an empty file, in which we will write the typedefs associated
        if (needToCreateTypedefs)
    {
        std::cerr << " -> Generating headers";
        //Find the files needed to be included
        printIncludes( category);

        //Get typedefs for the TYPE_DOUBLE
        printFullTypedefs( category, TYPE_DOUBLE);
        writeFile( category, TYPE_DOUBLE, sofaDouble);
        typedefComponents.clear();
        simplificationTypedefComponents.clear();

        //Get typedefs for the TYPE_FLOAT
        printFullTypedefs( category, TYPE_FLOAT);
        writeFile( category, TYPE_FLOAT, sofaFloat);
        typedefComponents.clear();
        simplificationTypedefComponents.clear();

        //Get typedefs for the TYPE_COMBINATION
        printFullTypedefs( category, TYPE_COMBINATION);
        writeFile( category, TYPE_COMBINATION, sofaTypedef);
        typedefComponents.clear();
        simplificationTypedefComponents.clear();

    }
        std::cerr << std::endl;
    }

        sofaTypedef << "\n#endif\n";
        sofaTypedef << "#endif\n";


        sofaTypedef << "#endif\n";
        sofaDouble  << "\n#endif\n";
        sofaFloat   << "\n#endif\n";

        return 0;
    }
