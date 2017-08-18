#include "support.h"

Support::Support(int type){
    Support::type = type;
}


void Support::generate(Slices& slices){
    switch (type)
    {
    case 0:
        break;
    case generalsupport:
        generateGeneralBranch(slices);
        break;
    case kbranchsupport:
        generateKBranch(slices);
        break;
    }
    printf ("support generation done\n");
}


/****************** Overhang Detection Step *******************/

// detects overhang regions in all layers
void overhangDetect(Slices& slices){
    Clipper clpr;
    clpr.ZFillFunction(&zfillone);

    Paths prev_sum;

    // calculate overhang regions
    for (int idx=slices.size()-1; idx>=0; idx--){
        qDebug() << "overhang region detect" << idx << "/" << slices.size();
        Slice& slice = slices[idx];
        Paths temp_overhang_region;

        // area subtraction
        clpr.Clear();
        clpr.AddPaths(prev_sum, ptSubject, true);
        clpr.AddPaths(slice.outershell, ptClip, true);
        clpr.Execute(ctDifference, slice.overhang_region, pftNonZero, pftNonZero);

        prev_sum = slice.outershell;

        // calculate critical overhang region
        getCriticalOverhangRegion(slice);

        // no critical overhang region so continue
        if (slice.critical_overhang_region.size() == 0){
            continue;
        }



        // poll normal of polygon
        for (int cop_idx=0; cop_idx<slice.critical_overhang_region.size(); cop_idx ++){
            Path cop = slice.critical_overhang_region[cop_idx];
            Paths partial_cops = areaSubdivision(cop,  pow(cfg->subdivision_radius*cfg->resolution,2)*cfg->support_density);
            for (int p_cop_idx=0; p_cop_idx<partial_cops.size(); p_cop_idx++){
                Path partial_cop = partial_cops[p_cop_idx];
                //IntPoint centroid = getPolygonCentroid(partial_cop); // need to change to this
                IntPoint centroid = getPolygonNormal(partial_cop);
//                qDebug() << slice.z << int(slice.z*Configuration::resolution) << slice.z*Configuration::resolution/Configuration::resolution;
                slices.overhang_points.push_back(OverhangPoint(centroid.X, centroid.Y, int(slice.z*cfg->resolution), cfg->default_support_radius));
                //slices.overhang_points.push_back(QVector3D(float(centroid.X)/Configuration::resolution, float(centroid.Y)/Configuration::resolution, slice.z));
            }
        }

        // poll centroid but using dupplication scheme
        /*for (int cop_idx=0; cop_idx<slice.critical_overhang_region.size(); cop_idx++){
            Path cop = slice.critical_overhang_region[cop_idx];

            float cop_area = Area(cop);
            // int dup_count = round(cop_area/40000); // best scenario
            int dup_count = round(cop_area/(pow(cfg->duplication_radius*cfg->resolution,2)));
            qDebug() << "dup_count : " <<dup_count;
            IntPoint centroid = getPolygonCentroid(cop);

            for (int op_idx=0; op_idx<dup_count; op_idx ++){
                slices.overhang_points.push_back(QVector3D(float(centroid.X)/Configuration::resolution, float(centroid.Y)/Configuration::resolution, slice.z));
            }
        }*/

    }

    // poll overhang positions among selected points

    /*vector<QVector3D> overhang_points;
    int support_count = cfg->support_density*100; // best scenario
//    int support_count = (pow(6, cfg->support_density)-1)/5 * slices.overhang_points.size(); // 100 must be relational to material's weight or size
    qDebug() << "polling " << support_count << "supports from " << slices.overhang_points.size();

    for (int idx=0; idx<support_count; idx++){
        int random_idx = rand() % slices.overhang_points.size();
        overhang_points.push_back(slices.overhang_points[random_idx]);
    }
    slices.overhang_points = overhang_points;*/

    clusterPoints(slices.overhang_points);


    printf("overhang region detection done\n");
}


// finds critical overhang region which exceeds threshold diagonal length
void getCriticalOverhangRegion(Slice& slice) {

    for (int contour_idx=0; contour_idx< slice.overhang_region.size(); contour_idx++){
        Path contour = slice.overhang_region[contour_idx];

        Path right_contour;
        Path left_contour;

        int z0count = 0;
        int max_z0count =0;

        int head0count = 0;
        int tail0count = 0;

        bool reverse = true;

        int ip_idx = 0;
        int start_ip_idx = 0;
        int end_ip_idx = contour.size()-1;
        int temp_start_ip_idx = 0;

        while(ip_idx < contour.size()){
            IntPoint ip = contour[ip_idx];
            if (ip.Z != 0)
                break;
            head0count ++;
            ip_idx ++;
        }

        ip_idx = contour.size();
        while(ip_idx >0 ){
            ip_idx --;
            IntPoint ip = contour[ip_idx];
            if (ip.Z != 0)
                break;
            tail0count ++;
        }

//        qDebug() << "head tail count : " << head0count << tail0count;
        start_ip_idx = head0count;
        end_ip_idx = contour.size()-tail0count-1;

        if (end_ip_idx<0)
            return;

        max_z0count = head0count + tail0count;

        temp_start_ip_idx = head0count;
        for (ip_idx=head0count; ip_idx<contour.size()-tail0count; ip_idx++){
            IntPoint ip = contour[ip_idx];
            if (ip.Z == 1){
                if (z0count > max_z0count){
                    max_z0count = z0count;
                    end_ip_idx = ip_idx;
                    start_ip_idx = temp_start_ip_idx;
                    reverse = false;
                }
                z0count =0;
                temp_start_ip_idx = ip_idx;
            } else {
                z0count ++;
            }
        }

        if (reverse){
            right_contour.insert(right_contour.end(), contour.begin()+end_ip_idx, contour.end());
            left_contour.insert(left_contour.end(), contour.begin()+start_ip_idx, contour.begin()+end_ip_idx);
            right_contour.insert(right_contour.end(), contour.begin(), contour.begin()+start_ip_idx);
        }  else {
            left_contour.insert(left_contour.end(), contour.begin()+end_ip_idx, contour.end());
            right_contour.insert(right_contour.end(), contour.begin()+start_ip_idx, contour.begin()+end_ip_idx);
            left_contour.insert(left_contour.end(), contour.begin(), contour.begin()+start_ip_idx);
        }

        /*if (left_contour.size() == 0){
            //left_contour.clear();
            left_contour.push_back(right_contour[right_contour.size()-1]);
            left_contour.push_back(right_contour[0]);
        } else if (left_contour.size() == 1){
            left_contour.push_back(right_contour[0]);
        }*/

        if (left_contour.size() <=1){
            left_contour.clear();
            left_contour.push_back(right_contour[right_contour.size()-1]);
            left_contour.push_back(right_contour[0]);

        }


        // get maximal hor_len so that we get only minimal vert_len
        float hor_len = 0;
        /*for (IntPoint ip1 : left_contour){
            for (IntPoint ip2 : left_contour){
                float distance = pointDistance(ip1, ip2);
                if (distance >= hor_len)
                    hor_len = distance;
            }
        }*/

        // faster but not precise
        hor_len = pointDistance(left_contour[0], left_contour[left_contour.size()-1]);

        float contour_area = Area(contour);

        float vert_len = contour_area/(hor_len*Configuration::resolution);

        if (vert_len >= cfg->overhang_threshold){
            slice.critical_overhang_region.push_back(contour);
        }


        /* perpendicular scheme

        if (right_contour.size() == 0){
            qDebug() << "it will not happen";
            return;
        } else if (right_contour.size() == 1){
            qDebug() << "it will not happen too";
            return;
        }
        if (left_contour.size() == 0){
            left_contour.push_back(right_contour[right_contour.size()-1]);
            left_contour.push_back(right_contour[0]);
        } else if (left_contour.size() == 1){
            left_contour.push_back(right_contour[right_contour.size()-1]);
        }

        // left right contour split done

        IntPoint prev_ip = right_contour[0];
        Path critical_right_path;
        Path critical_left_path;

        int support_dense = int(15/cfg->support_density);

        if (right_contour.size()/support_dense ==0){ // for small critical regions (including rectangles), search all the path
            prev_ip = right_contour[0];

            for (int ip_idx=0; ip_idx<right_contour.size(); ip_idx++){
                IntPoint ip = right_contour[ip_idx];
                if (ip_idx == 0)
                    continue;

                Path perpendicular;
                perpendicular.push_back(prev_ip);
                perpendicular.push_back(prev_ip-IntPoint(ip.Y-prev_ip.Y, prev_ip.X-ip.X));

                IntPoint left_hit;
                if (checkPerpendicularLength(perpendicular, left_contour, left_hit))
                {
                    // need to be sorted
                    critical_right_path.push_back(ip);
                    critical_left_path.push_back(left_hit);
                } else {
                    if (critical_right_path.size() != 0){
                        // merge critical_path
                        critical_right_path.insert(critical_right_path.end(), critical_left_path.begin(), critical_left_path.end());

                        slice.critical_overhang_region.push_back(critical_right_path);
                        critical_right_path.clear();
                        critical_left_path.clear();
                    }
                }

                prev_ip = ip;

            }
        }

        // collect last critical regions
        if (critical_right_path.size() != 0){
            // merge critical_path
            critical_right_path.insert(critical_right_path.end(), critical_left_path.begin(), critical_left_path.end());
            slice.critical_overhang_region.push_back(critical_right_path);
        }

        // clean buffer paths for next usage
        critical_right_path.clear();
        critical_left_path.clear();

        for (int ip_idx=0; ip_idx<=right_contour.size()/support_dense; ip_idx++){
            IntPoint ip = right_contour[ip_idx*support_dense];
            if (ip_idx == 0)
                continue;

            Path perpendicular;
            perpendicular.push_back(prev_ip);
            perpendicular.push_back(prev_ip-IntPoint(ip.Y-prev_ip.Y, prev_ip.X-ip.X));

            IntPoint left_hit;
            if (checkPerpendicularLength(perpendicular, left_contour, left_hit))
            {
                // need to be sorted
                critical_right_path.push_back(ip);
                critical_left_path.push_back(left_hit);
            } else {
                if (critical_right_path.size() != 0){
                    // merge critical_path
                    critical_right_path.insert(critical_right_path.end(), critical_left_path.begin(), critical_left_path.end());

                    slice.critical_overhang_region.push_back(critical_right_path);
                    critical_right_path.clear();
                    critical_left_path.clear();
                }
            }

            prev_ip = ip;

        }

        // collect last critical regions
        if (critical_right_path.size() != 0){
            // merge critical_path
            critical_right_path.insert(critical_right_path.end(), critical_left_path.begin(), critical_left_path.end());

            slice.critical_overhang_region.push_back(critical_right_path);
            critical_right_path.clear();
            critical_left_path.clear();
        }

        */

    }
}

/****************** Helper Functions For Overhang Detection Step *******************/

IntPoint lineIntersection(Path& A, Path& B) {
    // Store the values for fast access and easy
    // equations-to-code conversion
    float x1 = A[0].X, x2 = A[1].X, x3 = B[0].X, x4 = B[1].X;
    float y1 = A[0].Y, y2 = A[1].Y, y3 = B[0].Y, y4 = B[1].Y;

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // If d is zero, there is no intersection
    if (d==0) return NULL;

    // Get the x and y
    float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
    float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
    float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

    // Check if the x and y coordinates are within both lines
    float x_max = max(abs(x1), abs(x2));
    x_max = max(x_max, abs(x3));
    x_max = 2*max(x_max, abs(x4));

    float y_max = max(abs(y1), abs(y2));
    y_max = max(y_max, abs(y3));
    y_max = 2*max(y_max, abs(y4));

    // Remove outliers
    if ( x < -x_max || x > x_max ) return NULL;
    if ( y < -y_max || y > y_max ) return NULL;

    // Return the point of intersection
    IntPoint ret;
    ret.X = x;
    ret.Y = y;
    return ret;
}

float pointDistance(IntPoint A, IntPoint B){
    return sqrt(pow(A.X-B.X, 2)+pow(A.Y-B.Y,2));
}

float pointDistance(QVector3D A, QVector3D B){
    return sqrt(pow(A.x()-B.x(), 2)+pow(A.y()-B.y(),2)+pow(A.z()-B.z(),2));
}

// if slice contains overhang_point in somewhere
bool checkInclusion(Slice& slice, OverhangPoint overhang_point){
    for (Path contour : slice){
        if (PointInPolygon(overhang_point.position, contour)){
            return true;
        }
    }
    return false;

}

Path drawCircle(OverhangPoint overhang_point, int radius){
    Path circle;
    int circle_resolution = 8;
    float circle_radius = radius; //cfg->default_support_radius;
    float unit_angle = PI*2/circle_resolution;
    float angle = PI*2;

    for (int i=0; i<circle_resolution; i++){
        angle += unit_angle;
        IntPoint circle_point = overhang_point.position + IntPoint(int(circle_radius*cos(angle)), int(circle_radius*sin(angle)), overhang_point.position.Z);
        circle.push_back(circle_point);
    }
    return circle;
}

bool checkPerpendicularLength(Path A, Path B, IntPoint& left_hit){

    IntPoint prev_ip = B[0];

    for (int idx=0; idx<B.size(); idx++){
        IntPoint ip = B[idx];
        if (idx == 0)
            continue;

        Path B_part;
        B_part.push_back(prev_ip);
        B_part.push_back(ip);

        // check intersection

        IntPoint li = lineIntersection(A,B_part);
        if (li != NULL){
            if (pointDistance(A[0], li)/Configuration::resolution > cfg->overhang_threshold){
                left_hit = li;
                return true;
            }
        } else {
//            qDebug() << "no line intersection" << A[0].X << A[0].Y << A[1].X << A[1].Y << prev_ip.X << prev_ip.Y << ip.X << ip.Y;
        }

        prev_ip = ip;
    }

    return false;
}

IntPoint getPolygonNormal(Path vertices){
    IntPoint normal;
    for (IntPoint vertice : vertices){
        normal.X+=vertice.X;
        normal.Y+=vertice.Y;
    }
    normal.X /= vertices.size();
    normal.Y /= vertices.size();
    return normal;
}


IntPoint getPolygonCentroid(Path vertices)
{
    int vertexCount = vertices.size();

    IntPoint centroid;
    float signedArea = 0.0;
    float x0 = 0.0; // Current vertex X
    float y0 = 0.0; // Current vertex Y
    float x1 = 0.0; // Next vertex X
    float y1 = 0.0; // Next vertex Y
    float a = 0.0;  // Partial signed area

    // For all vertices
    int i=0;
    for (i=0; i<vertexCount; i++)
    {
        x0 = vertices[i].X;
        y0 = vertices[i].Y;
        x1 = vertices[(i+1) % vertexCount].X;
        y1 = vertices[(i+1) % vertexCount].Y;
        a = x0*y1 - x1*y0;
        signedArea += a;
        centroid += IntPoint((x0 + x1) *a,(y0 + y1) *a);
    }

    if (signedArea == 0 || vertexCount <= 3){
        centroid = getPolygonNormal(vertices);
    } else {
        signedArea *= 0.5;
        centroid /= (6.0*signedArea);
    }
    /*if (!PointInPolygon(centroid, vertices)){
        centroid = vertices[vertices.size()/2];
    }*/
    return centroid;
}

// subdivides area into reasonable size
Paths areaSubdivision(Path area, float criterion){
    float area_size = abs(Area(area));
//    qDebug() << "areasubdivision" << area_size << criterion;
    Paths result;
    if ((area_size >criterion) && area.size()>=4){
        int half_size = (area.size()/2);
        IntPoint begin_point = area[0];
        IntPoint end_point = area[area.size()-1];
        IntPoint half_point = area[half_size-1];
        IntPoint half_next_point = area[half_size];
        IntPoint A_point = (begin_point + end_point)/2; // check out 연구노트
        IntPoint B_point = (half_point + half_next_point)/2;

        Path left_half;
        left_half.push_back(A_point);
        left_half.insert(left_half.end(), area.begin(), area.begin() + half_size);
        left_half.push_back(B_point);

        Path right_half;
        right_half.push_back(B_point);
        right_half.insert(right_half.end(), area.begin() + half_size, area.end());
        right_half.push_back(A_point);

//        qDebug() << "left right half size : " << left_half.size() << right_half.size() << area.size();

        for (Path path : areaSubdivision(left_half, criterion)){
            result.push_back(path);
        }
        for (Path path : areaSubdivision(right_half, criterion)){
            result.push_back(path);
        }
    } else {
        result.push_back(area);
    }
    return result;
}

void clusterPoints(vector<OverhangPoint>& points){
    vector<OverhangPoint> unclassified_points;
    vector<OverhangPoint> classified_points;

    OverhangPoint container_point;
    int container_count;
    float container_size = 10000;//(1-cfg->support_density)*10;

    unclassified_points = points;
    container_point = unclassified_points[unclassified_points.size()-1];
    unclassified_points.pop_back();
    classified_points.push_back(container_point);

    vector<OverhangPoint>::iterator it;
    int total_size = unclassified_points.size();
    while (total_size>0){
        //qDebug() << "unclassified_points size : " << unclassified_points.size();
        for (it = unclassified_points.begin(); it != unclassified_points.end();){
            OverhangPoint point = (*it);
            if (pointDistance(point.position, container_point.position) <= cfg->duplication_radius*Configuration::resolution){
                container_count ++;
                unclassified_points.erase(it);
            } else {
                ++it;
            }
        }
        total_size = unclassified_points.size();
        if (total_size >=1){
            container_count = 0;
            container_point = unclassified_points[total_size-1];
            unclassified_points.pop_back();
            classified_points.push_back(container_point);
        }
    }
    points = classified_points;
    qDebug() << "clustered points" << points.size();
}

//****************** overhang position initializers *******************/

void OverhangPoint::branchTo(IntPoint target){
    branching_overhang_point->position = IntPoint(target.X,target.Y,target.Z);
}
