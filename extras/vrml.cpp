// Demo rules set for VRML supplied by Dave Handley.
// Intended for testing only.

#include "stdafx.h"

#include "../lexertl/rules.hpp"
#include "vrml.h"

void build_vrml (lexertl::rules &rules_)
{
    rules_.push("Group", eGroup);
    rules_.push("Separator", eSeparator);
    rules_.push("Switch", eSwitch);
    rules_.push("TransformSeparator", eTransformSeparator);
    rules_.push("WWWAnchor", eWWWAnchor);
    rules_.push("ShapeHints", eShapeHints);
    rules_.push("MatrixTransform", eMatrixTransform);
    rules_.push("Rotation", eRotation);
    rules_.push("Scale", eScale);
    rules_.push("Transform", eTransform);
    rules_.push("Translation", eTranslation);
    rules_.push("Material", eMaterial);
    rules_.push("IndexedFaceSet", eIndexedFaceSet);
    rules_.push("Coordinate3", eCoordinate3);
    rules_.push("Texture2", eTexture2);
    rules_.push("TextureCoordinate2", eTextureCoordinate2);
    rules_.push("Normal", eNormal);
    rules_.push("NormalBinding", eNormalBinding);
    rules_.push("DEF", eDEF);
    rules_.push("USE", eUSE);
    rules_.push("point", epoint);
    rules_.push("coordIndex", ecoordIndex);
    rules_.push("materialIndex", ematerialIndex);
    rules_.push("normalIndex", enormalIndex);
    rules_.push("textureCoordIndex", etextureCoordIndex);
    rules_.push("matrix", ematrix);
    rules_.push("rotation", erotation);
    rules_.push("scaleFactor", escaleFactor);
    rules_.push("vertexOrdering", evertexOrdering);
    rules_.push("shapeType", eshapeType);
    rules_.push("faceType", efaceType);
    rules_.push("creaseAngle", ecreaseAngle);
    rules_.push("filename", efilename);
    rules_.push("image", eimage);
    rules_.push("wrapS", ewrapS);
    rules_.push("wrapT", ewrapT);
    rules_.push("translation", etranslation);
    rules_.push("scaleOrientation", escaleOrientation);
    rules_.push("center", ecenter);
    rules_.push("ambientColor", eambientColor);
    rules_.push("diffuseColor", ediffuseColor);
    rules_.push("specularColor", especularColor);
    rules_.push("emissiveColor", eemissiveColor);
    rules_.push("shininess", eshininess);
    rules_.push("transparency", etransparency);
    rules_.push("vector", evector);
    rules_.push("value", evalue);
    rules_.push("UNKNOWN_ORDERING", eUNKNOWN_ORDERING);
    rules_.push("CLOCKWISE", eCLOCKWISE);
    rules_.push("COUNTERCLOCKWISE", eCOUNTERCLOCKWISE);
    rules_.push("UNKNOWN_SHAPE_TYPE", eUNKNOWN_SHAPE_TYPE);
    rules_.push("SOLID", eSOLID);
    rules_.push("UNKNOWN_FACE_TYPE", eUNKNOWN_FACE_TYPE);
    rules_.push("CONVEX", eCONVEX);
    rules_.push("REPEAT", eREPEAT);
    rules_.push("CLAMP", eCLAMP);
    rules_.push("DEFAULT", eDEFAULT);
    rules_.push("OVERALL", eOVERALL);
    rules_.push("PER_PART", ePER_PART);
    rules_.push("PER_PART_INDEXED", ePER_PART_INDEXED);
    rules_.push("PER_FACE", ePER_FACE);
    rules_.push("PER_FACE_INDEXED", ePER_FACE_INDEXED);
    rules_.push("PER_VERTEX", ePER_VERTEX);
    rules_.push("PER_VERTEX_INDEXED", ePER_VERTEX_INDEXED);

    rules_.push("\\{", eOpenCurly);
    rules_.push("\\}", eCloseCurly);
    rules_.push("\\[", eOpenSquare);
    rules_.push("\\]", eCloseSquare);

//        rules_.push("([+/-]?((([0-9]+(\\.?)|([0-9]*\\.[0-9]+))([eE][+\\-]?[0-9]+)?))"), 69);

    rules_.push("([-+]?((([0-9]+[.]?|([0-9]*[.][0-9]+))([eE][-+]?[0-9]+)?)))", eFloat);
    rules_.push("#[^\\n\\r]*[\\n\\r]", eComment);
    rules_.push("\"[^\"\\n\\r]*[\"\\n\\r]", eString);
    rules_.push("[a-zA-Z_][a-zA-Z0-9_]*", eID);
    rules_.push("[ \\t\\n\\r]+", eWhitespace);
    rules_.push("[,]", eComma);
    rules_.push(".", eUnknown);
}
