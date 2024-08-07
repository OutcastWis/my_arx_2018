#include "pch.h"
#include "command.h"

#include <rxobject.h>
#include <rxregsvc.h>
#include <rxmfcapi.h>
#include <aced.h>
#include <dbelipse.h>
#include <dbsymtb.h>
#include <geassign.h>
#include <dbapserv.h>
#include <dbgroup.h>
#include <acestext.h>

#include <tchar.h>

#include <AcExtensionModule.h>

#include "MyAcUiDialog.h"
#include "MyTxtStyle.h"
#include "MyTessellate.h"
#include "MyInventoryData.h"
#include "cmd_count.h"
#include "ex_dict.h"
#include "highlight_subentity.h"
#include "raster_image.h"
#include "context_menu.h"
#include "docman.h"
#include "extend_tabs.h"
#include "modeless_dialog.h"
#include "ssget.h"
#include "protocol_reactor.h"
#include "block_order.h"
#include "clone_work.h"
#include "jig.h"
#include "data_per_doc.h"
#include "osnap.h"
#include "custom_geo.h"


AC_DECLARE_EXTENSION_MODULE(theArxDLL);


void register_class() {
    MyTxtStyle::rxInit();
    MyTessellate::rxInit();
    MyInventoryData::rxInit();

    acrxBuildClassHierarchy();
}

void unregister_class() {
    deleteAcRxClass(MyTxtStyle::desc());
    deleteAcRxClass(MyTessellate::desc());
    deleteAcRxClass(MyInventoryData::desc());


    // Medium
    wzj::cmd_count::instance()->stop();
    wzj::ex_dict::instance()->stop();
    wzj::highlight_subentity::instance()->stop();
    wzj::raster_image::instance()->stop();
    wzj::context_menu::instance()->stop();
    wzj::extend_tabs::instance()->stop();
    wzj::modeless_dialog::instance()->stop();

    // High
    wzj::docman::instance()->stop();
    wzj::block_order::instance()->stop();
    wzj::clone_work::instance()->stop();
    wzj::jig::instance()->stop();
    wzj::data_per_doc::instance()->stop();
    wzj::osnap::instance()->stop();

    // Advanced
    wzj::custom_geo::instance()->stop();
}

void init_app(void* appId) {

    register_class();

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_TEST"), _T("LOCAL_TEST"), 
        ACRX_CMD_MODAL | ACRX_CMD_REDRAW , test);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_CURVES"), _T("LOCAL_CURVES"), ACRX_CMD_MODAL, curves);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_MKENTS"), _T("LOCAL_MKENTS"), ACRX_CMD_MODAL, mk_ents);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_MODEL_DIALOG"), _T("LOCAL_MODEL_DIALOG"), ACRX_CMD_MODAL, model_dialog,
        NULL, -1, theArxDLL.ModuleResourceInstance());

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_PROG_METER"), _T("LOCAL_PROG_METER"), ACRX_CMD_MODAL, progress_meter);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_CUSTOM_TEXT"), _T("LOCAL_CUSTOM_TEXT"), ACRX_CMD_MODAL, custom_text);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_COMPLEX_LAYER"), _T("LOCAL_COMPLEX_LAYER"), ACRX_CMD_MODAL, complex_layer);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_CMD_COUNT"), _T("LOCAL_CMD_COUNT"), ACRX_CMD_MODAL, cmd_count);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_HIGHLIGHT"), _T("LOCAL_HIGHLIGHT"), ACRX_CMD_MODAL, highlight);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_EXDICT"), _T("LOCAL_EXDICT"), ACRX_CMD_MODAL, ex_dict);
    
    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_RASTER"), _T("LOCAL_RASTER"), ACRX_CMD_MODAL, raster_image);
    // 函数extend_tabs,的前提. 为option对话框注册. 这边的第一个参数会写入注册表中当前配置下OptionsDialog\TabExtensions中. 但除此之外不知道有何作用, 似乎可以任意取名
    acedRegisterExtendedTab(_T("arx2018.arx"), _T("OptionsDialog"));

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_MODELESS"), _T("LOCAL_MODELESS"), ACRX_CMD_MODAL, modeless_dialog);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_SSGET"), _T("LOCAL_SSGET"), ACRX_CMD_MODAL, ssget);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_PR"), _T("LOCAL_PR"), ACRX_CMD_MODAL, pr);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_DOCMAN"), _T("LOCAL_DOCMAN"), ACRX_CMD_MODAL, docman);

    context_menu(appId); // 注册用户自己的快捷菜单

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_BLOCK_ORDER"), _T("LOCAL_BLOCK_ORDER"), ACRX_CMD_MODAL, block_order);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_CLONE_WORK"), _T("LOCAL_CLONE_WORK"), ACRX_CMD_MODAL, clone_work);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_JIG"), _T("LOCAL_JIG"), ACRX_CMD_MODAL, jig);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_DPDOC"), _T("LOCAL_DPDOC"), ACRX_CMD_MODAL, data_per_doc);

    acedRegCmds->addCommand(_T("WZJ_COMMAND_GROUP"), _T("GLOBAL_OSNAP"), _T("LOCAL_OSNAP"), ACRX_CMD_MODAL, osnap);
}


void unload_app() {
    acedRegCmds->removeGroup(_T("WZJ_COMMAND_GROUP"));

    unregister_class();
}


Acad::ErrorStatus add_to_model_space(AcDbObjectId& objId, AcDbEntity* pEntity, AcDbDatabase* pDb) {
    AcDbBlockTable* pBlockTable;
    AcDbBlockTableRecord* pSpaceRecord;

    if (pDb == nullptr)
        acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
    else
        pDb->getSymbolTable(pBlockTable, AcDb::kForRead);

    pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForWrite);
    pBlockTable->close();

    auto es = pSpaceRecord->appendAcDbEntity(objId, pEntity);
    pSpaceRecord->close();

    return es;
}


void create_layer(const TCHAR* layer_name, bool frozon, bool off) {
    AcDbLayerTable* pLayerTable;
    auto es = acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForWrite);
    if (es != Acad::eOk) {
        ads_printf(_T("\n[ERROR] Failed to open symbol table. Error: %s"), acadErrorStatusText(es));
    }

    AcDbLayerTableRecord* pLayerTableRecord = nullptr;
    if (pLayerTable->getAt(layer_name, pLayerTableRecord, AcDb::kForWrite) != Acad::eOk) {
        pLayerTableRecord = new AcDbLayerTableRecord;
        pLayerTableRecord->setName(layer_name);
        pLayerTable->add(pLayerTableRecord);
    }

    pLayerTable->close();

    pLayerTableRecord->setIsFrozen(frozon);
    pLayerTableRecord->setIsOff(off);
    pLayerTableRecord->close();
}


AcDbObjectId create_line() {
    AcGePoint3d startPt(4.0, 2.0, 0.0);
    AcGePoint3d endPt(10.0, 7.0, 0.0);
    AcDbLine* pLine = new AcDbLine(startPt, endPt);

    AcDbBlockTable* pBlockTable;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);

    AcDbBlockTableRecord* pBlockTableRecord;
    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
        AcDb::kForWrite);
    pBlockTable->close();

    AcDbObjectId lineId;
    pBlockTableRecord->appendAcDbEntity(lineId, pLine);

    pBlockTableRecord->close();
    pLine->close();

    return lineId;
}


AcDbObjectId create_circle() {
    AcGePoint3d center(9.0, 3.0, 0.0);
    AcGeVector3d normal(0.0, 0.0, 1.0);
    AcDbCircle* pCirc = new AcDbCircle(center, normal, 2.0);

    AcDbBlockTable* pBlockTable;
    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);

    AcDbBlockTableRecord* pBlockTableRecord;
    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
        AcDb::kForWrite);
    pBlockTable->close();

    AcDbObjectId circleId;
    pBlockTableRecord->appendAcDbEntity(circleId, pCirc);

    pBlockTableRecord->close();
    pCirc->close();

    return circleId;
}


void create_group(AcDbObjectIdArray& objIds, const TCHAR* pGroupName) {
    AcDbGroup* pGroup = new AcDbGroup(pGroupName);

    // Put the group in the group dictionary which resides
    // in the named object dictionary.
    //
    AcDbDictionary* pGroupDict;
    acdbHostApplicationServices()->workingDatabase()->getGroupDictionary(pGroupDict, AcDb::kForWrite);

    AcDbObjectId pGroupId;
    pGroupDict->setAt(pGroupName, pGroup, pGroupId);
    pGroupDict->close();

    // Now that the group has been added, it has an ObjectID.
    // This is important since the group will become a persistent
    // reactor for the added entities...
    for (int i = 0; i < objIds.length(); i++) {
        pGroup->append(objIds[i]);
    }

    pGroup->close();
}


Acad::ErrorStatus change_color(AcDbObjectId entId, Adesk::UInt16 newColor)
{
    AcDbEntity* pEntity;
    acdbOpenObject(pEntity, entId, AcDb::kForWrite);

    pEntity->setColorIndex(newColor);
    pEntity->close();

    return Acad::eOk;
}

Adesk::Boolean getYorN(const TCHAR* pStr)
{
    TCHAR yorn_str[132];

    // specific prompt.
    acutPrintf(_T("\n%s"), pStr);

    acedInitGet(0, _T("否 是 _No Yes"));

    yorn_str[0] = _T('Y');
    yorn_str[1] = _T('\0');

    switch (acedGetString(Adesk::kFalse, _T(" -- No/<Yes>:  "),
        yorn_str)) {
    case RTKWORD:
        acedGetInput(yorn_str);
        /* Deliberate fallthrough */
    default:
        break;
    }

    return (!((yorn_str[0] == _T('N')) || (yorn_str[0] == _T('n'))));
}

CString desktop_url() {
    TCHAR szDesktopPath[MAX_PATH] = {};
    SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, szDesktopPath);
    return szDesktopPath;
}

int get_int_sysvar(const TCHAR* var)
{
    struct resbuf rb;
    acedGetVar(var, &rb);
    return(rb.resval.rint);
}

void set_int_sysvar(const TCHAR* var, int value)
{
    struct resbuf rb;
    rb.restype = RTSHORT;
    rb.resval.rint = value;
    acedSetVar(var, &rb);
}

AcDbObjectIdArray ContainerIdsAndEntity(ads_name sset, ads_point pick_point, Adesk::GsMarker* gs, AcDbObjectId* pick)
{
    AcDbObjectIdArray ids;
    AcDbObjectId id;
    Adesk::Int32 len = 0;
    acedSSLength(sset, &len);
    for (long i = 0; i < len; i++)
    {
        resbuf* rb = NULL;
        if (RTNORM == acedSSNameX(&rb, sset, i))
        {
            resbuf* rbWalk = rb;

            // skip RTLB
            rbWalk = rbWalk->rbnext;
            // select method
            ads_printf(_T("selectin method: %d\n"), rbWalk->resval.rint);
            // picked entity's entity name
            rbWalk = rbWalk->rbnext;
            acdbGetObjectId(*pick, rbWalk->resval.rlname);
            ids.append(*pick);
            // GS maker
            rbWalk = rbWalk->rbnext;
            *gs = rbWalk->resval.rint;
            // RTLB
            rbWalk = rbWalk->rbnext;
            // point descriptor
            rbWalk = rbWalk->rbnext;
            auto pt_desc = rbWalk->resval.rint;
            // point on pick line (in WCS)
            rbWalk = rbWalk->rbnext;
            ads_point_set(rbWalk->resval.rpoint, pick_point);

            while (NULL != rbWalk)
            {
                if (RTENAME == rbWalk->restype)
                { // 这些对象, 按照嵌套深度, 由内至外
                    AcDbObjectId nested_id;
                    acdbGetObjectId(nested_id, rbWalk->resval.rlname);
                    ids.append(nested_id);
                }
                rbWalk = rbWalk->rbnext;
            }
            acutRelRb(rb);
        }
    }
    ids.reverse(); // 翻转, 为外部AcDbFullSubentPath使用
    return ids;
}


void curves() {
    ads_name en = {};
    AcGePoint3d pt;
    if (ads_entsel(_T("\nSelect an Ellipse: "), en, asDblArray(pt)) != RTNORM)
    {
        acutPrintf(_T("\nNothing selected"));
        return;
    }

    AcDbObjectId eId;
    acdbGetObjectId(eId, en);

    AcDbObject* pObj;
    acdbOpenObject(pObj, eId, AcDb::kForRead);
    if (pObj->isKindOf(AcDbEllipse::desc())) {
        pObj->close();
        acutPrintf(_T("\n[OK] Selected an ellipse"));
    }
    else {
        pObj->close();
        acutPrintf(_T("\n[WARNING] Selected entity is not an ellipse"));
    }
}


void mk_ents() {
    ads_printf(_T("\n[INFO] Create a layer called WZJ_MYLAYER"));
    create_layer(_T("WZJ_MYLAYER"),false, false);

    AcDbObjectIdArray idArr;
    idArr.append(create_line());
    idArr.append(create_circle());

    ads_printf(_T("\n[INFO] Change the color of circle to Index 1"));
    change_color(idArr.last(), 1);

    ads_printf(_T("\n[INFO] Make a group called WZJ_MYGROUP"));
    create_group(idArr, _T("WZJ_MYGROUP"));
}


void model_dialog() {
    MyAcUiDialog dlg(CWnd::FromHandle(adsw_acadMainWnd()));
    INT_PTR nReturnValue = dlg.DoModal();
}


void progress_meter() {
    acutPrintf(_T("Running Progress Meter...\n"));

    acedSetStatusBarProgressMeter(_T("Test Progress Bar"), 0, 100);
    for (int i = 0; i <= 100; i++) {
        Sleep(100);
        acedSetStatusBarProgressMeterPos(i);
    }
    acedRestoreStatusBar();
}


void custom_text() {
    // 获取text style
    AcDbTextStyleTable* pStyleTable;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pStyleTable, AcDb::kForRead);

    AcDbTextStyleTableIterator* pIterator;
    pStyleTable->newIterator(pIterator);
    // get ObjectId of first record in table
    AcDbTextStyleTableRecord* style = nullptr;
    pIterator->getRecord(style);
    delete pIterator;
    pStyleTable->close();

    // AcDbTextStyleTableRecord 转 AcGiTextStyle
    AcGiTextStyle ts;
    {
        const TCHAR* pTmpStr;
        style->fileName(pTmpStr);
        ts.setFileName(pTmpStr);

        style->bigFontFileName(pTmpStr);
        ts.setBigFontFileName(pTmpStr);

        ts.setTextSize(style->textSize());
        ts.setXScale(style->xScale());
        ts.setObliquingAngle(style->obliquingAngle());

        style->close();
        ts.loadStyleRec(); // 该函数最后调用
    }


    MyTxtStyle::SetStyle(ts);
    MyTxtStyle* mts = new MyTxtStyle;

    AcDbObjectId id;
    add_to_model_space(id, mts);
    mts->close();
}

void complex_layer() {
    AcDbLayerTable* pLayerTbl;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTbl, AcDb::kForWrite);

    if (!pLayerTbl->has(_T("WZJ_COMPLEX_LAYER"))) {
        AcDbLayerTableRecord* pLayerTblRcd = new AcDbLayerTableRecord;
        pLayerTblRcd->setName(_T("WZJ_COMPLEX_LAYER"));
        pLayerTblRcd->setIsFrozen(0);// layer to THAWED
        pLayerTblRcd->setIsOff(0);   // layer to ON
        pLayerTblRcd->setVPDFLT(0);  // viewport default
        pLayerTblRcd->setIsLocked(0);// un-locked

        AcCmColor color;
        color.setColorIndex(1); // set color to red
        pLayerTblRcd->setColor(color);

        // For linetype, we need to provide the object ID of
        // the linetype record for the linetype we want to
        // use.  First, we need to get the object ID.
        AcDbLinetypeTable* pLinetypeTbl;
        AcDbObjectId ltId;
        acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLinetypeTbl, AcDb::kForRead);
        if ((pLinetypeTbl->getAt(_T("DASHED"), ltId)) != Acad::eOk)
        {
            acutPrintf(_T("\nUnable to find DASHED linetype. Using CONTINUOUS"));

            // CONTINUOUS is in every drawing, so use it.
            pLinetypeTbl->getAt(_T("CONTINUOUS"), ltId);
        }
        pLinetypeTbl->close();

        pLayerTblRcd->setLinetypeObjectId(ltId);
        pLayerTbl->add(pLayerTblRcd);
        pLayerTblRcd->close();
        pLayerTbl->close();
    }
    else {
        pLayerTbl->close();
        acutPrintf(_T("\nlayer WZJ_COMPLEX_LAYER already exists"));
    }
}


void tessellate() {
    MyTessellate* t = new MyTessellate;

    AcDbObjectId id;
    add_to_model_space(id, t);
    t->close();
}

void cmd_count() {
    if (wzj::cmd_count::instance()->is_start())
        wzj::cmd_count::instance()->stop();
    else
        wzj::cmd_count::instance()->init();
}

void ex_dict() {
    if (wzj::ex_dict::instance()->is_start())
        wzj::ex_dict::instance()->stop();
    else
        wzj::ex_dict::instance()->init();
}

void highlight() {
    if (wzj::highlight_subentity::instance()->is_start())
        wzj::highlight_subentity::instance()->stop();
    else
        wzj::highlight_subentity::instance()->init();
}

void raster_image() {
    if (wzj::raster_image::instance()->is_start())
        wzj::raster_image::instance()->stop();
    else
        wzj::raster_image::instance()->init();
}

void context_menu(void* appId) {

    wzj::context_menu::instance()->set_data(appId);
    wzj::context_menu::instance()->init();
}

void extend_tabs(void* appId) {
    wzj::extend_tabs::instance()->add_tab(appId);
}

void modeless_dialog() {
    if (wzj::modeless_dialog::instance()->is_start())
        wzj::modeless_dialog::instance()->stop();
    else
        wzj::modeless_dialog::instance()->init();
}

void ssget() {
	if (wzj::ssget::instance()->is_start())
		wzj::ssget::instance()->stop();
	else
		wzj::ssget::instance()->init();
}

void pr() {
    if (wzj::protocol_reactor::instance()->is_start())
        wzj::protocol_reactor::instance()->stop();
    else
        wzj::protocol_reactor::instance()->init();
}

void docman() {
    if (wzj::docman::instance()->is_start())
        wzj::docman::instance()->stop();
    else
        wzj::docman::instance()->init();
}

void block_order() {
    if (wzj::block_order::instance()->is_start())
        wzj::block_order::instance()->stop();
    else
        wzj::block_order::instance()->init();
}

void clone_work() {
    if (wzj::clone_work::instance()->is_start())
        wzj::clone_work::instance()->stop();
    else
        wzj::clone_work::instance()->init();
}

void jig() {
    if (wzj::jig::instance()->is_start())
        wzj::jig::instance()->stop();
    else
        wzj::jig::instance()->init();
}

void data_per_doc() {
    if (wzj::data_per_doc::instance()->is_start())
        wzj::data_per_doc::instance()->stop();
    else
        wzj::data_per_doc::instance()->init();
}

void osnap() {
    if (wzj::osnap::instance()->is_start())
        wzj::osnap::instance()->stop();
    else
        wzj::osnap::instance()->init();
}

void custom_geo() {
    if (wzj::custom_geo::instance()->is_start())
        wzj::custom_geo::instance()->stop();
    else
        wzj::custom_geo::instance()->init();
}