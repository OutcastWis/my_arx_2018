### ��Ҫͷ�ļ�
```c++
#include <rxobject.h>
#include <rxregsvc.h>
#include <aced.h>
#include <acedads.h>	// acedȫ�ֺ���
#include <dbelipse.h>
#include <dbsymtb.h>
#include <adslib.h>		// RTNORM, acdb, acutȫ�ֺ���
#include <geassign.h>
#include <dbapserv.h>	// acdbSetHostApplicationServices
#include <dbents.h>		// ����ʵ��
```


### ��ͼ, AcGi
* CAD�ж���AcGiͼ����ʾʵ�ֹ淶
1. **2D-display pipe**����������ʾ����Ļ�ϵ�2Dͼ��
2. **3D system**���ڳ־ó��ֵ���ͼ
3. **proxy generation**. �ڱ����Զ������ʱʹ�ã���ͼ�δ洢��Ԫ�ļ��У���Ԫ�ļ��������Զ���Ӧ�ó��򲻿��õ�ϵͳ�ϼ���ͼ��ʱ�ز�
4. **explode operation**. �Ѷ���ը����ObjectDBX����ͼԪ�����ڷ��ָ��Ӷ���ı߽�

* AcGiWorldDraw::regenType()
1. **kAcGiStandardDisplay** ���͵Ļ�ͼģʽ�����û�����REGEN�����༭��Ŀʱʹ�á�������ģʽ�£�ʵ��Ӧ�����߿��г���
2. **kAcGiHideOrShadeCommand** ִ��������ɾ������ָʾHIDE��SHADE������Ч��������ģʽ�£�ʵ��Ӧ��ʹ��������Ⱦ��
3. **kAcGiShadedDisplay** ʹ�ò��Ϻ�����������3Dģ�͵ı�����Ӱͼ�񣬲����û�����RENDER����ʱʹ�á�������ģʽ�£�ʵ��Ӧ��ʹ��������Ⱦ
4. **kAcGiForExplode** ���ڱ�ը����.
5. **kAcGiSaveWorldDrawForProxy** �������ɴ���ͼ�Ρ�����������£����е���Ⱦ��Ӧ����subWorldDraw()����ɣ���ΪsubViewportDraw()��֧�ִ���ͼ�Ρ�
6. **eAcGiRegenTypeInvalid** ��Ч


### Ƕ�׶���, nested entity
* Ƕ�׶���ĸ�ʽ��: block ref -> block ref -> ... -> entity
* Ƕ�׶���Ĳ�����Ҫʹ��AcDbFullSubentPath. ����ʹ�õ��������û�����, ��������������, �������ֵ�BUG. �������ʱ, ��������λ�������޷������ĸ�����.
* ����face, edge, vertex��, �ڴ˻�����, ��Ҫʹ��getSubentPathsAtGsMarker
* ���highlight_subentity


### ��ݲ˵�, AcEdUiContext
* ��5��:
1. **Hot Grip Cursor** �Ҽ�ѡ�ж����grip
2. **Object Snap Cursor** ��סshift�����Ҽ�
3. **Default Mode** ���޶���ѡ��, Ҳ����������ʱ, �Ҽ�
4. **Command Mode** ��������ʱ�Ҽ�
5. **Edit Mode** ����������, ѡ�ж����, ��û��ѡ��grip, �Ҽ�

* ��ݲ˵���cui�����ж�Ӧ�ı���
1. Ĭ�ϲ˵�: POP501, CMDEFAULT
2. �༭�˵�: POP502, CMEDIT
3. ����˵�: POP503, CMCOMMAND
�������߱������, �����Ӧ������ģʽ�޷�ʹ��. ����, cui�в�����POP501, CMDEFAULTʱ, Default Mode�㲻��ʹ��, ��ʹ�ڳ����н�����ע��
* ���context_menu

* ����ģʽ�ܷ���ȷ��ʾ���������Ҽ�������. ��options->�û�ϵͳ����->�Զ����Ҽ�����->����ģʽ ��, ���ѡ�����"��ݲ˵�: ����ѡ�����ʱ����", ����û�п��õ�ѡ��ʱ���Ҽ��밴 Enter ����Ч��һ����. ѡ��"��ݲ˵�: ��������"�����.
* "����ѡ�����ʱ����"��Ҫ��ʹ��acedget*��, �Բ���prompt�����趨. prompt��ʽ����:
```
Description: 
Current instruction (descriptive info) or 
[Option1/oPtion2/opTion3/...] <default option>:
```
> ��Ҫ�ǵ�3����, ��option������. �����Ŀ�������. ע��ÿ�в�Ҫ��80���ַ�, ��Ҫ����3��.
* ���context_menu.cpp��detail::cmd����


### CTRL+C work as cancel or copy ?
* CTRL+C��cad2018�ϵı����Ǹ���. ��cui��ȡ���ļ��̿�ݷ�ʽ��, CTRL+C���κ�Ч��. ��֪�����ʵ�ְ����ĵ���CTRL+C����cancel��Ч��. ����˾����������CTRL+C����ȡ��, ��Ч����ESC��ͬ. CTRL+Cȡ����, ��Ҫ��acedUsrBrk������, �����Ӱ��ͬ�������һ��acedget*, ����ֱ�ӷ���RTCAN. ��ESC�򲻻�, ������ҪacedUsrBrk���в���


### �ĵ������ĵ��л�
* �ɰ汾CAD��fiber, �л��ĵ��ᵼ��ԭ�ĵ����������. �����ڵİ汾����, ����2018����, ����ǰ�ĵ�����ٹ���, ����ִ����ɺ�, �ڽ���ʵ�ʵ��ĵ��л�. 
* �����½�, ��, ����ȶ�����Ϊ�ĵ������ĵ��л�
* **current document**ָ�������ڲ����������������ĵ�, ͨ��curDocument��ȡ
* **activated document**ָ���ǽ��������ϲ���ĵ�, ͨ��mdiActiveDocument��ȡ
* �������߿��Բ�ͬ. ������һ���ĵ�ʱ, ��һ����current document. ��������ͨ��setCurDocument���иı�, �Ӷ�ʹ���߲�ͬ. �˳�����ʱ, ȷ������ͳһ



### ע���
* ��ACRX_DXF_DEFINE_MEMBERS�ĵ����ڶ���һ��Ҫ��д
* AcFile::fprintf����BUG. 64λ��, ������ȷ���wchar_t. fprintf(L"%s", L"ABC"), ֻ����ʾA. ����setCharFormatΪUTF8��������Ч, ����ʹ�����е�FILE*����attach���ɲ���
* acedSSGet��"C"��"W"ģʽ, ��Ҫ��ѡ��������ͼ��, ���򷵻�RTERROR, ������ѡ�ж���
* �����������Ҫʹ��MFC��Դʱ, ȷ����addCommand��������ʱ, ��AC_DECLARE_EXTENSION_MODULE. ����������ʵ����, ��CAcModuleResourceOverride