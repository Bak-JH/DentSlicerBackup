#include "autoorientation.h"
#include <qDebug>
#include <qmlmanager.h>


namespace autoorientationPrivate
{
	float target_function(float touching, float overhang, float line);
	float* lithograph(const Mesh* mesh, float n[], float amin, int CA);
	float get_touching_line(const Mesh* mesh, float a[], const MeshFace& face, float touching_height);
	std::vector<Orient*> area_cumulation(const Mesh* mesh, float n[], bool bi_algorithmic);
	std::vector<Orient*> egde_plus_vertex(const Mesh* mesh, int bsvest_n);
	float* calc_random_normal(const Mesh* mesh, int i, const MeshFace& face);
	std::vector<Orient*> remove_duplicates(std::vector<Orient*> o, int* orientCnt);
	rotateResult* euler(Liste bestside);
}

using namespace autoorientationPrivate;
Orient::Orient(float _val, QVector3D _label)
{
    val = _val;
    label = _label;
}

/*
The critical angle CA is a variable that can be set by the operator as
    it may depend on multiple factors such as material used, printing
     temperature, printing speed, etc.
*/

rotateResult* autoorientation::Tweak(const Mesh* mesh, bool bi_algorithmic,int CA,bool *appropriately_rotated){
    qmlManager->setProgress(0);
    qmlManager->setProgressText("startttttt");

    float n[]={0.0, 0.0, 1.0};
    int best_n; //orientation의 갯수, 여기 갯수에서 +1 한 것이 총 orientation 갯수가 됩니다.
    int additional_n=12; //orientation갯수를 더 늘리고 싶을 때 사용
    //(최댓값이라 원하는 만큼 늘어나진 않습니다.egde_plus_vertex 참고)

    if(bi_algorithmic)
        best_n=7;
    else
        best_n=5;
    std::vector<Liste> liste(best_n+additional_n);//orientation 의 계산값들을 저장및 출력하기 위해 사용
    std::vector<Orient*> orientations=area_cumulation(mesh, n,bi_algorithmic);
    if (orientations.size() < best_n) best_n = orientations.size();

    //bi_algorithmic이 false: 6개, true:8개의 orientation 값이 리턴됩니다.
    int orientCnt=0;
    //edge_plus_vertex로 추가된 orient갯수 카운터

    if(bi_algorithmic){
          std::vector<Orient*> plusVertex=egde_plus_vertex(mesh,additional_n);
          //additional_n 이하의 갯수의 orientation이 추가됩니다.

          orientations.insert(orientations.end(),plusVertex.begin(), plusVertex.end());
          //추가된 orientation을 원본 뒤에 붙입니다.
          qDebug()<<"total orientation: "<<orientCnt+8;
          orientations=remove_duplicates(orientations,&orientCnt);
          //orientation의 중복을 제거합니다. 그래서 orientation의 최솟값이 8이 아니라
          //더 떨어질 수도 있습니다.
          //free(plusVertex);
    }
    best_n+=orientCnt;

    //갯수 업데이트, false일 때는 값 변화가 없습니다.
    for(int i=0; i<best_n+1;i++){
        float orientation[] ={-orientations[i]->label[0],
                             -orientations[i]->label[1],
                             -orientations[i]->label[2]};
        float amin=approachvertex(mesh,orientation);
        float *temp=lithograph(mesh,orientation,amin,CA);
        //bottom,overhang,line을 구합니다.
        float bottomA=temp[0];
        float overhangA=temp[1];
        float lineL=temp[2];

        //구한 값을 liste에 추가합니다.
        liste[i].orientation.setX(orientation[0]);
        liste[i].orientation.setY(orientation[1]);
        liste[i].orientation.setZ(orientation[2]);
        liste[i].bottomA=bottomA;
        liste[i].overhangA=overhangA;
        liste[i].lineL=lineL;
        liste[i].isActive=true;
        //free(temp);
        qmlManager->setProgress(i*0.04/(best_n+1)+0.95);
    }

    qmlManager->setProgress(0.99);
    qmlManager->setProgressText("99");
    bool minFlag=true;
    //qt에서 최댓값이 얼마인지 몰라 flag로 만들었습니다.
    float Unprintability;
    // 프린트 불가능성. 이게 최소가 되는 orientation을 선택합니다.
    std::vector<float> tempUnprintability(best_n+2); //출력을 위해 임시저장용로 만든 것입니다.
    Liste bestside; //Unprintability가 최소일 때 orientation과 bottom,hangover,line을 저장합니다.
    for(int i=0;i<best_n+1;i++){
        float F = target_function(liste[i].bottomA,liste[i].overhangA,liste[i].lineL);
        //계산된 bottom,overhang,line으로 unprintability를 계산합니다.
        if(liste[i].orientation.z()==1||liste[i].orientation.z()==-1){
            continue;
        }
        tempUnprintability[i]=F;
        if(minFlag){
            Unprintability=F;
            bestside.orientation.setX(liste[i].orientation.x());
            bestside.orientation.setY(liste[i].orientation.y());
            bestside.orientation.setZ(liste[i].orientation.z());
            bestside.bottomA=liste[i].bottomA;
            bestside.overhangA=liste[i].overhangA;
            bestside.lineL=liste[i].lineL;
            bestside.isActive=true;
            minFlag=false;
        }else if (F<Unprintability - 0.05){
            Unprintability=F;
            bestside.orientation.setX(liste[i].orientation.x());
            bestside.orientation.setY(liste[i].orientation.y());
            bestside.orientation.setZ(liste[i].orientation.z());
            bestside.bottomA=liste[i].bottomA;
            bestside.overhangA=liste[i].overhangA;
            bestside.lineL=liste[i].lineL;
            bestside.isActive=true;
        }
        //저장.
    }
    rotateResult* result;// Tweak의 return값입니다. axis,phi,R로 구성되어있습니다.
    if(bestside.isActive){
        *appropriately_rotated=true;
        result=euler(bestside);
    }else{
        *appropriately_rotated=false;
        qDebug() << "?";
        qmlManager->setProgress(1);
        qmlManager->setProgressText("Done");

        qmlManager->openResultPopUp("","Orientation Done","");
        return NULL;
    }
    //━━━━━━━━━━━━━━━━━━━━ 내용 출력 ━━━━━━━━━━━━━━━━━━━
    if(false){//필요 시 verbose를 추가해도 됩니다.
        qDebug()<<"Examine "<<best_n+1<<" orientations:";
        qDebug("  %-35s%-16s%-16s%-16s%-16s","Area Vector:","Touching Area:","Overhang:","Line length:","Unprintability:");
        for(int i=0;i<best_n+1;i++){
            qDebug("  [%-9f, %-9f, %-9f]  %-16f%-16f%-16f%-16f",
                   liste[i].orientation.x(),
                   liste[i].orientation.y(),
                   liste[i].orientation.z(),
                   liste[i].bottomA,
                   liste[i].overhangA,
                   liste[i].lineL,
                   tempUnprintability[i]);
        }
        qDebug("\nResult-stats:");
        qDebug("**[0,0,1] or [0,0,-1] is ignored**");
        qDebug(" Tweaked Z-axis:        [%-9f, %-9f, %-9f]",bestside.orientation.x(),bestside.orientation.y(),bestside.orientation.z());
        qDebug()<<" Axis, angle:           "<<result->v<<" "<<result->phi;
        qDebug(" Rotation matrix:");
        qDebug()<<result->R;

        qDebug(" Unprintability:        %-9f",Unprintability);
    }
    //━━━━━━━━━━━━━━━━━━━━ 내용 출력 ━━━━━━━━━━━━━━━━━━━
    //free(orientations);
    qmlManager->setProgress(1);
    qmlManager->setProgressText("Done");

    qmlManager->openResultPopUp("","Orientation Done","");
    return result;
}
float autoorientationPrivate::target_function(float touching,float overhang,float line){
    float ABSLIMIT=100;
    float RELLIMIT=1;
    float LINE_FAKTOR = 0.5; //원문 그대로 적었습니다. FACTOR가 아닐까 생각듭니다.
    float touching_line=line*LINE_FAKTOR;
    float F = (overhang/ABSLIMIT) + (overhang / (touching+touching_line));
    return F;
}
float autoorientation::approachvertex(const Mesh* mesh,float n[]){
    bool minFlag=true;
    //qt에서 최댓값이 얼마인지 몰라 flag로 만들었습니다.

    float amin;
	for (const auto& face : mesh->getFaces())
	{
		auto idx = face.meshVertices();
		//한 삼각형의 세 점 index를 받아옵니다.
		float a1 = idx[0]->position.x() * n[0] +
			idx[0]->position.y() * n[1] +
			idx[0]->position.z() * n[2];
		float a2 = idx[1]->position.x() * n[0] +
			idx[1]->position.y() * n[1] +
			idx[1]->position.z() * n[2];
		float a3 = idx[2]->position.x() * n[0] +
			idx[2]->position.y() * n[1] +
			idx[2]->position.z() * n[2];
		//orientation의 방향과 계산을 한 뒤 최솟값을 구합니다.
		//예상으론 oriestation방향으로 들어오는 평면과 가장 가까운 점의 거리를 구하는게 아닐까 싶습니다.
		float an = std::min(std::min(a1, a2), a3);

		if (minFlag) {
			amin = an;
			minFlag = false;
		}
		else if (an < amin) {
			amin = an;
		}
	}


    return amin;
}
float* autoorientationPrivate::lithograph(const Mesh* mesh, float n[], float amin, int CA){
    //overhang,bottom,line을 구합니다.
    float Overhang=1;
    float alpha= - cos((90-CA)*M_PI/180);  //CA는 0~90도입니다 기본은 45도.
    float bottomA=1;
    float LineL=1;
    float touching_height = amin+15;

    float anti_n[3]={-n[0],-n[1],-n[2]};

	for (const auto& face : mesh->getFaces())
	{
        QVector3D a = face.fn_unnorm;//face에 추가된 인스턴스이며, 정규화되지 않은 노말벡터입니다.
        float norma = (float)sqrtf(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
        if(norma == 0)
            continue;
        if(alpha > (a[0]*n[0] + a[1]*n[1] +a[2]*n[2])/norma){
			auto idx = face.meshVertices();
            float a1 = idx[0]->position.x()*n[0]+
                    idx[0]->position.y()*n[1]+
                    idx[0]->position.z()*n[2];
            float a2 = idx[1]->position.x()*n[0]+
                    idx[1]->position.y()*n[1]+
                    idx[1]->position.z()*n[2];
            float a3 = idx[2]->position.x()*n[0]+
                    idx[2]->position.y()*n[1]+
                    idx[2]->position.z()*n[2];
            float an = std::min(std::min(a1,a2),a3);
            float ali=fabs(a.x()*n[0]+a.y()*n[1]+a.z()*n[2])/2;


            if(touching_height < an){
                if(0.00001 < fabs(a.x()-anti_n[0])+fabs(a.y()-anti_n[1])+fabs(a.z()-anti_n[2])){
                    ali = 0.8 * ali;
                }
                Overhang += ali;
            }
            else{
                float b[3]={a1,a2,a3};
                bottomA += ali;
                LineL += get_touching_line(mesh,b,face,touching_height);
            }
        }
    }
    float *temp=(float*)malloc(sizeof(float)*3);
    //세 인자 전달을 위한 임시 배열입니다.
    temp[0]=bottomA;
    temp[1]=Overhang;
    temp[2]=LineL;
    return temp;
}
float autoorientationPrivate::get_touching_line(const Mesh* mesh,float a[], const MeshFace& face,float touching_height){
    std::vector<const MeshVertex*> touch_list;
	auto idx = face.meshVertices();
    for(int j=0;j<3;j++){
        if(a[j]<touching_height){
            touch_list.push_back(idx[j].operator->());
        }
    }
	std::vector<const MeshVertex*> combs;
    for(int j=0;j<touch_list.size();j++){
        const MeshVertex* a1;
		const MeshVertex* a2;
        a1=touch_list.at(j);
        for(int k=j+1;k<touch_list.size();k++){
            a2=touch_list.at(k);
            combs.push_back(a1);
			combs.push_back(a2);
        }
    }
    if(combs.size() <= 1){
        return 0;
    }
    float length = 0;
    for(int j=0;j<combs.size();j++){
		const MeshVertex* p1=combs.at(j);
        j++;
		const MeshVertex* p2=combs.at(j);
        length+=sqrtf(        (p2->position.x()-p1->position.x())*(p2->position.x()-p1->position.x())+
                              (p2->position.y()-p1->position.y())*(p2->position.y()-p1->position.y())+
                              (p2->position.z()-p1->position.z())*(p2->position.z()-p1->position.z()));
    }
    return length;
}
std::vector<Orient*> autoorientationPrivate::area_cumulation(const Mesh* mesh,float n[],bool bi_algorithmic){
    //bi_algorithmic이 false:6개 true:8개의 orientataion*을 만들어 리턴합니다.
    int best_n;
    if(bi_algorithmic)
        best_n=7;
    else
        best_n=5;
    //5,7 로 써있지만, (0,0,1)한개 더 처음에 추가합니다.
    std::map<QString,float> orient;
    //리턴되는 값은 단순 random은 아니며, 가장 가중치가 높은 순서대로 best_n개를 뽑아냅니다.
	size_t i = 0;
	for (const auto& face : mesh->getFaces())
	{
        QVector3D an = face.fn_unnorm;
        float A = sqrtf(an.x()*an.x()+an.y()*an.y()+an.z()*an.z());
        if(A>0){
            an.setX(an.x()/A);
            an.setY(an.y()/A);
            an.setZ(an.z()/A);
            QString key=QString::number(QVariant(an.x()).toFloat())+","+
                    QString::number(QVariant(an.y()).toFloat())+","+
                    QString::number(QVariant(an.z()).toFloat());
            //이렇게 비효율적으로 변환한 이유는, 이상하게 qt에서는 round를 써서 소숫점을 지우려고 해도
            //지워지지 않았기 때문입니다. 원문에서는 소숫점6자리의 tuple(x,y,z)를 key로 만들었고,
            //c++에서는 이것이 안되므로 하나의 string으로 변환하여 사용했습니다.

            if(orient[key]==NULL){
                orient[key]=A;
            }else{
                orient[key]=orient[key]+A;
            }//해당 위치에 가중치를 더합니다.

        }
		//?!
        if(i%3000==0){
            qmlManager->setProgress((float)i*0.15/mesh->getFaces().size());
            qmlManager->setProgressText("orientation.....");
        }
		++i;
    }

    qmlManager->setProgress(0.15);
    qmlManager->setProgressText("orientation.....");
    //map에 있는 값들의 value를 오름차순해서, 상위 best_n개를 뽑아야합니다.
    //python 원문에서는 Counter(map).most_common(best_n)으로 간단히 구현되는 것이지만,
    //역시 c++에는 없으므로 만들었습니다.

    if (orient.size() < best_n) best_n = orient.size();

    std::map<QString,float>::iterator it_map;
    std::vector<float> val(best_n);
	std::vector <QString> val_n(best_n);
    for(int i=0;i<best_n;i++){
        val[i]=0;
        val_n[i]="";
    }

    for(it_map=orient.begin(); it_map !=orient.end() ; ++it_map){
        if(it_map->second > val[best_n-1]){
            int index=best_n-2;
            float tar=it_map->second;
            for(int i=index ; i>=0 ; i--){
                if(tar>val[i]){
                    index--;
                }
                else{
                    break;
                }
            }
            index++;
            for(int i=best_n-1;i>index;i--){
                val[i]=val[i-1];
                val_n[i]=val_n[i-1];
            }
            val[index]=it_map->second;
            val_n[index]=it_map->first;
        }
    }
    std::vector<Orient*> result;

    result.push_back(new Orient(0, QVector3D(n[0], n[1], n[2])));

    //0,0,1을 처음에 추가합니다.

    for(int i=1;i<best_n+1;i++){
        QStringList temp=val_n[i-1].split(',');
        QVector3D temp_vec = QVector3D(0,0,0);
        if (temp.size() >= 3){
            temp_vec = QVector3D(temp[0].toFloat(), temp[1].toFloat(), temp[2].toFloat());
        }
        result.push_back(new Orient (val[i-1],temp_vec));

    }

    //가장 가중치가 높은 orientation들도 추가합니다.
    return result;
}
std::vector<Orient*> autoorientationPrivate::egde_plus_vertex(const Mesh* mesh, int best_n){
    //orientation을 추가로 더할 때 씁니다. best_n 갯수만큼 return 하긴 하지만,
    //호출한 Tweak() 에서 val>2인 orientation만 원본 orientation에 추가하므로,
    //실제로 추가되는 값은 그보다 작을 수 있습니다.
    int vcount = mesh->getFaces().size()*3;
    //원문에서는 face 안에 vertexindex가 있다고 생각하지 않고,
    //vertexlist를 계속 나열한 채로 저장하였으므로, 비슷하게 구현하기 위해서 *3을 해줍니다.
    //그냥 다른 함수처럼 faces[i].mesh_vertex[0] 1, 2 이런식으로 구한 결과와 같습니다.
    int it=0;
    if(vcount < 10000) it = 5;
    else if(vcount < 25000) it = 2;
    else it = 1;
    std::map<QString,float> lst;
    //area_cummulation처럼 map을 만들고, value 기준 상위 best_n개 만 뽑아냅니다.
    //단, map을 만드는 요소를 mesh 내에서 random하게 추가로 생성합니다.
	auto faceItr = mesh->getFaces().cbegin();
    for(int i=0;i<vcount*it;i++){
        float* randomNormal = calc_random_normal(mesh,i%vcount, *faceItr);
        //mesh 속 삼각형 중에서 랜덤 하게 하나를 골라 Normal을 전달받습니다.
        if(randomNormal==NULL){//randomNormal 크기가 0이 되는 경우를 제외합니다.
            continue;
        }
        //받은 값으로 map을 만들어줍니다.
        QString key=QString::number(QVariant(randomNormal[0]).toFloat())+","+
                QString::number(QVariant(randomNormal[1]).toFloat())+","+
                QString::number(QVariant(randomNormal[2]).toFloat());
        lst[key]=lst[key]+it;

        //free(randomNormal);
        if(i%3000==0){
            qmlManager->setProgress((float)i*0.80/(vcount*it)+0.15);
            qmlManager->setProgressText("orientation.....");
        }
		if (faceItr != mesh->getFaces().cend())
			++faceItr;
		else
			break;
    }

    qmlManager->setProgress(0.95);
    qmlManager->setProgressText("orientation.....");
    std::map<QString,float>::iterator it_map;
    std::vector<float> val(best_n);
    std::vector<QString> val_n(best_n);
    for(int i=0;i<best_n;i++){
        val[i]=0;
        val_n[i]="";
    }
    for(it_map=lst.begin(); it_map !=lst.end() ; ++it_map){
        if(it_map->second > val[best_n-1]){
            int index=best_n-2;
            float tar=it_map->second;
            for(int i=index ; i>=0 ; i--){
                if(tar>val[i]){
                    index--;
                }
                else{
                    break;
                }
            }
            index++;
            for(int i=best_n-1;i>index;i--){
                val[i]=val[i-1];
                val_n[i]=val_n[i-1];
            }
            val[index]=it_map->second;
            val_n[index]=it_map->first;
        }
    }
    std::vector<Orient*> result;

    for (int i=0; i<best_n;i++){
        QStringList temp=val_n[i].split(',');
        QVector3D temp_vec = QVector3D(0,0,0);
        if (temp.size() >= 3){
            temp_vec = QVector3D(temp[0].toFloat(), temp[1].toFloat(), temp[2].toFloat());
        }
        result.push_back(new Orient(val[i], temp_vec));
    }

    //추가할 orientation best_n개를 리턴합니다.
    return result;

}
float* autoorientationPrivate::calc_random_normal(const Mesh* mesh, int i,  const MeshFace& face){
    //mesh 내의 random한 normal을 리턴합니다.
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    QVector3D v;
    QVector3D w;
	int j = i / 3;
    //if(i%3 == 0){

    //    v=face.mesh_vertex[0]->position;
    //    w=face.mesh_vertex[1]->position;
    //}
    //else if(i%3 == 1){
    //    v=face.mesh_vertex[1]->position;
    //    w=face.mesh_vertex[2]->position;
    //}
    //else{
    //    v=face.mesh_vertex[2]->position;
    //    w=face.mesh_vertex[0]->position;
    //}
	auto currFace = mesh->getFaces()[j];
	auto meshVertices = currFace.meshVertices();
	if (i % 3 == 0) {
		v = meshVertices[0]->position;
		w = meshVertices[1]->position;
	}
	else if (i % 3 == 1) {
		v = meshVertices[1]->position;
		w = meshVertices[2]->position;
	}	
	else { 
		v = meshVertices[2]->position;
		w = meshVertices[0]->position;
	}

    int randomIndex=qrand() % mesh->getFaces().size();
	QVector3D r_v = mesh->getFaces()[randomIndex / 3].meshVertices()[randomIndex % 3]->position;

    //QVector3D r_v=mesh->vertices[mesh->faces[randomIndex/3].mesh_vertex[randomIndex%3]].position;

	//temp
	//QVector3D r_v = mesh->getVertices()->front().position;
    v.setX(v.x()-r_v.x());
    v.setY(v.y()-r_v.y());
    v.setZ(v.z()-r_v.z());
    w.setX(w.x()-r_v.x());
    w.setY(w.y()-r_v.y());
    w.setZ(w.z()-r_v.z());
    float* a=((float*)malloc(sizeof(float)*3));
    a[0]=v.y()*w.z() - v.z()*w.y();
    a[1]=v.z()*w.x() - v.x()*w.z();
    a[2]=v.x()*w.y() - v.y()*w.x();
    float n = sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    if( n != 0){ //크기가 1로 정규화합니다.
        a[0]=a[0]/n;
        a[1]=a[1]/n;
        a[2]=a[2]/n;

        return a;
    }
    else{
        return NULL;
    }
}
std::vector<Orient*> autoorientationPrivate::remove_duplicates(std::vector<Orient*> o,int *orientCnt){
    //중복을 제거합니다.
    int initIndex=*orientCnt+8;
    int count=0;
    std::vector<Orient*> orientation;
    for (int i=0; i<initIndex;i++){
        orientation.push_back(new Orient(0,QVector3D(0,0,0)));
    }

    //Orient* orientation=(Orient*)malloc(sizeof(Orient)*(initIndex));
    orientation[count]=o[count];
    count++;
    for(int i=1;i<initIndex;i++){
        bool overlapFlag = false;
        for(int j=0; j<count;j++){
            //orientation이 같은 지 여부는, 유클리드 거리로 측정합니다.
            float dif=sqrtf((o[i]->label.x()-orientation[j]->label.x())*(o[i]->label.x()-orientation[j]->label.x())
                            +(o[i]->label.y()-orientation[j]->label.y())*(o[i]->label.y()-orientation[j]->label.y())
                            +(o[i]->label.z()-orientation[j]->label.z())*(o[i]->label.z()-orientation[j]->label.z()));
            if(dif<0.001){
                overlapFlag=true;
                break;
                //중복이 생기면 저장하지 않습니다.
            }
        }
        if(overlapFlag==false){
            orientation[count]=o[i];
            count++;
        }

    }
    qDebug()<<"delete "<<initIndex-count<<" overlapped orientation";
    *orientCnt=count-8;
    return orientation;
}
rotateResult* autoorientationPrivate::euler(Liste bestside){
    //bestside의 orientation, bottom,overhang,line 으로 unprintability를 계산합니다.
    QVector3D v;
    float phi;
    if(bestside.orientation.x()==0 && bestside.orientation.y()==0 && bestside.orientation.z()==-1){
        v.setX(1);
        v.setY(0);
        v.setZ(0);
        phi=M_PI; //3.14
    }
    else if(bestside.orientation.x()==0 && bestside.orientation.y()==0 && bestside.orientation.z()==1){
        v.setX(1);
        v.setY(0);
        v.setZ(0);
        phi=0;
    }
    else{
        phi=M_PI - acosf(-bestside.orientation.z());
        v.setX(-bestside.orientation.y());
        v.setY(bestside.orientation.x());
        v.setZ(0);

        float norm=sqrtf(v.x()*v.x() + v.y()*v.y() + v.z()*v.z());
        v.setX(v.x()/norm);
        v.setY(v.y()/norm);
        v.setZ(v.z()/norm);
    }
    float R[16]={v.x()*v.x()*(1-cosf(phi)) + cosf(phi),
        v.x()*v.y()*(1-cosf(phi)) - v.z()*sinf(phi),
        v.x()*v.z()*(1-cosf(phi)) + v.y()*sinf(phi),
        0,
        v.y()*v.x()*(1-cosf(phi)) + v.z()*sinf(phi),
        v.y()*v.y()*(1-cosf(phi)) + cosf(phi),
        v.y()*v.z()*(1-cosf(phi)) - v.x()*sinf(phi),
        0,
        v.z()*v.x()*(1-cosf(phi)) - v.y()*sinf(phi),
        v.z()*v.y()*(1-cosf(phi)) + v.x()*sinf(phi),
        v.z()*v.z()*(1-cosf(phi)) + cosf(phi),
        0,
        0,
        0,
        0,
        1
       };
    QMatrix4x4 RMatrix=QMatrix4x4((qreal)R[0],(qreal)R[1],(qreal)R[2],(qreal)R[3],
            (qreal)R[4],(qreal)R[5],(qreal)R[6],(qreal)R[7],
            (qreal)R[8],(qreal)R[9],(qreal)R[10],(qreal)R[11],
            (qreal)R[12],(qreal)R[13],(qreal)R[14],(qreal)R[15]);
//    QMatrix4x4 RMatrix=QMatrix4x4((qreal)-1,(qreal)0,(qreal)0,(qreal)0,
//                                  (qreal)0,(qreal)1,(qreal)0,(qreal)0,
//                                  (qreal)0,(qreal)0,(qreal)1,(qreal)0,
//                                  (qreal)0,(qreal)0,(qreal)0,(qreal)1);

    rotateResult* result=new rotateResult();
    result->phi=phi;
    result->v=v;
    result->R=RMatrix;
    return result;
}


