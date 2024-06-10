#include "class.h"

object* obj_init()
{
    object* newobj = new object();
    newobj->v_list = new std::vector<vertex*>();
    newobj->e_list = new std::vector<edge*>();
    newobj->f_list = new std::vector<face*>();
    newobj->vertices = new std::vector<GLfloat>();
    newobj->vertexIndices = new std::vector<unsigned int>();
    return newobj;
}

vertex* vtx_init()
{
    vertex* newvtx = new vertex();
    newvtx->e_list = new std::vector<edge*>();
    newvtx->f_list = new std::vector<face*>();
    newvtx->v_new = NULL;
    newvtx->idx = -1;
    return newvtx;
}

edge* edge_init()
{
    edge* newedge = new edge();
    newedge->f_list = new std::vector<face*>();
    newedge->v1 = NULL;
    newedge->v2 = NULL;
    newedge->edge_pt = NULL;
    return newedge;
}

face* face_init()
{
    face* newface = new face();
    newface->v_list = new std::vector<vertex*>();
    newface->e_list = new std::vector<edge*>();
    newface->face_pt = NULL;
    return newface;
}

vertex* add_vertex(object* obj, const coord& coord)
{
    vertex* newvtx = vtx_init();
    newvtx->xyz.x = coord.x;
    newvtx->xyz.y = coord.y;
    newvtx->xyz.z = coord.z;
    newvtx->idx = obj->v_list->size();
    obj->v_list->push_back(newvtx);
    return newvtx;
}

edge* find_edge(object* obj, vertex* v1, vertex* v2)
{
    std::vector<edge*>* v1_edgeList = v1->e_list;
    for (int i = 0; i < v1_edgeList->size(); i++)
    {
        if ((*v1_edgeList)[i]->v1 == v2 || (*v1_edgeList)[i]->v2 == v2)
        {
            return (*v1_edgeList)[i];
        }
    }
    return NULL;
}

vertex* find_v(object* obj, vertex* v1)
{
    std::vector<vertex*>* v1_edgeList = obj->v_list;
    for (int i = 0; i < v1_edgeList->size(); i++)
    {
        if ((*v1_edgeList)[i]->xyz.x == v1->xyz.x&& (*v1_edgeList)[i]->xyz.y == v1->xyz.y&& (*v1_edgeList)[i]->xyz.z == v1->xyz.z)
        {
            //printf("test \n");
            return (*v1_edgeList)[i];
        }
    }
    //printf("test2 \n");
    return NULL;
}

edge* add_edge(object* obj, vertex* v1, vertex* v2)
{
    edge* newedge = edge_init();
    newedge->v1 = v1;
    newedge->v2 = v2;
    v1->e_list->push_back(newedge);
    v2->e_list->push_back(newedge);
    obj->e_list->push_back(newedge);
    return newedge;
}

face* add_face(object* obj, const std::vector<int>& vertexIndices)
{
    face* newface = face_init();
    int n = vertexIndices.size();
    for (int i = 0; i < n; i++)
    {
        vertex* v1 = (*(obj->v_list))[vertexIndices[i]];
        vertex* v2 = (*(obj->v_list))[vertexIndices[(i + 1) % n]];
        v1->f_list->push_back(newface);

        edge* temp = find_edge(obj, v1, v2);
        if (!temp) temp = add_edge(obj, v1, v2);

        temp->f_list->push_back(newface);
        newface->e_list->push_back(temp);
        newface->v_list->push_back(v1);
    }
    obj->f_list->push_back(newface);
    return newface;
}

coord add(const coord& ord1, const coord& ord2)
{
    coord temp;
    temp.x = ord1.x + ord2.x;
    temp.y = ord1.y + ord2.y;
    temp.z = ord1.z + ord2.z;
    return temp;
}

coord sub(const coord& ord1, const coord& ord2)
{
    coord temp;
    temp.x = ord1.x - ord2.x;
    temp.y = ord1.y - ord2.y;
    temp.z = ord1.z - ord2.z;
    return temp;
}

coord mul(const coord& ord1, GLfloat m)
{
    coord temp;
    temp.x = ord1.x * m;
    temp.y = ord1.y * m;
    temp.z = ord1.z * m;
    return temp;
}

coord div(const coord& ord1, GLfloat d)
{
    coord temp;
    temp.x = ord1.x / d;
    temp.y = ord1.y / d;
    temp.z = ord1.z / d;
    return temp;
}

coord cross(const coord& ord1, const coord& ord2)
{
    coord temp;
    temp.x = ord1.y * ord2.z - ord1.z * ord2.y;
    temp.y = ord1.z * ord2.x - ord1.x * ord2.z;
    temp.z = ord1.x * ord2.y - ord1.y * ord2.x;
    return temp;
}

void setNorm(object* obj)
{
    for (int i = 0; i < obj->f_list->size(); i++)
    {
        face* temp = (*(obj->f_list))[i];
        coord v01 = sub((*(temp->v_list))[1]->xyz, (*(temp->v_list))[0]->xyz);
        coord v12 = sub((*(temp->v_list))[2]->xyz, (*(temp->v_list))[1]->xyz);
        coord crs = cross(v01, v12);
        crs.normalize();
        temp->norm = crs;
    }

    for (int i = 0; i < obj->v_list->size(); i++)
    {
        coord sum;
        std::vector<face*>* temp = (*(obj->v_list))[i]->f_list;
        int n = temp->size();
        for (int j = 0; j < n; j++)
        {
            sum.add((*temp)[j]->norm);
        }
        sum.div((GLfloat)n);
        sum.normalize();
        (*(obj->v_list))[i]->avg_norm = sum;
    }
}

void aggregate_vertices(object* obj)
{
    obj->vertices->clear();
    obj->vertexIndices->clear();

    for (int i = 0; i < obj->v_list->size(); i++)
    {
        coord temp_pos = (*(obj->v_list))[i]->xyz;
        coord temp_norm = (*(obj->v_list))[i]->avg_norm;
        obj->vertices->push_back(temp_pos.x);
        obj->vertices->push_back(temp_pos.y);
        obj->vertices->push_back(temp_pos.z);
        obj->vertices->push_back(temp_norm.x);
        obj->vertices->push_back(temp_norm.y);
        obj->vertices->push_back(temp_norm.z);
    }

    if (obj->vertices_per_face == 3)
    {
        for (int i = 0; i < obj->f_list->size(); i++)
        {
            std::vector<vertex*>* temp = (*(obj->f_list))[i]->v_list;
            obj->vertexIndices->push_back((*temp)[0]->idx);
            obj->vertexIndices->push_back((*temp)[1]->idx);
            obj->vertexIndices->push_back((*temp)[2]->idx);
        }
    }

    else if (obj->vertices_per_face == 4)
    {
        for (int i = 0; i < obj->f_list->size(); i++)
        {
            std::vector<vertex*>* temp = (*(obj->f_list))[i]->v_list;
            obj->vertexIndices->push_back((*temp)[0]->idx);
            obj->vertexIndices->push_back((*temp)[1]->idx);
            obj->vertexIndices->push_back((*temp)[2]->idx);
            obj->vertexIndices->push_back((*temp)[2]->idx);
            obj->vertexIndices->push_back((*temp)[3]->idx);
            obj->vertexIndices->push_back((*temp)[0]->idx);
        }
    }
}

object* cube()
{
    object* newobj = obj_init();
    newobj->vertices_per_face = 4;
    for (int x = -1; x <= 1; x += 2)
    {
        for (int y = -1; y <= 1; y += 2)
        {
            for (int z = -1; z <= 1; z += 2)
            {
                add_vertex(newobj, coord((GLfloat)x, (GLfloat)y, (GLfloat)z));
            }
        }
    }
    add_face(newobj, { 0,2,6,4 });
    add_face(newobj, { 0,4,5,1 });
    add_face(newobj, { 0,1,3,2 });
    add_face(newobj, { 2,3,7,6 });
    add_face(newobj, { 6,7,5,4 });
    add_face(newobj, { 1,5,7,3 });

    setNorm(newobj);

    aggregate_vertices(newobj);

    return newobj;
}


object* donut()
{
    object* m = obj_init();
    m->vertices_per_face = 4;
    int i;
    coord v[] = {
       { -2, -.5, -2 }, { -2, -.5,  2 }, {  2, -.5, -2 }, {  2, -.5,  2 },
       { -1, -.5, -1 }, { -1, -.5,  1 }, {  1, -.5, -1 }, {  1, -.5,  1 },
       { -2,  .5, -2 }, { -2,  .5,  2 }, {  2,  .5, -2 }, {  2,  .5,  2 },
       { -1,  .5, -1 }, { -1,  .5,  1 }, {  1,  .5, -1 }, {  1,  .5,  1 },
    };

    for (i = 0; i < 16; i++) add_vertex(m, coord(v[i].x, v[i].y, v[i].z));
    add_face(m, { 4, 5, 1, 0 });
    add_face(m, { 3, 1, 5, 7 });
    add_face(m, { 0, 2, 6, 4 });
    add_face(m, { 2, 3, 7, 6 });

    add_face(m, { 8, 9, 13, 12 });
    add_face(m, { 15, 13, 9, 11 });
    add_face(m, { 12, 14, 10, 8 });
    add_face(m, { 14, 15, 11, 10 });

    add_face(m, { 0, 1, 9, 8 });
    add_face(m, { 1, 3, 11, 9 });
    add_face(m, { 2, 0, 8, 10 });
    add_face(m, { 3, 2, 10, 11 });

    add_face(m, { 12, 13, 5, 4 });
    add_face(m, { 13, 15, 7, 5 });
    add_face(m, { 14, 12, 4, 6 });
    add_face(m, { 15, 14, 6, 7 });

    setNorm(m);

    aggregate_vertices(m);

    return m;
}

object* star()
{
    object* m = obj_init();
    m->vertices_per_face = 3;
    int ang, i;
    double rad;
    coord v[15];

    for (i = 0; i < 5; i++) {
        ang = i * 72;
        rad = ang * 3.1415926 / 180;
        v[i].x = 2.2 * cos(rad); v[i].y = 2.2 * sin(rad); v[i].z = 0;

        rad = (ang + 36) * 3.1415926 / 180;
        v[i + 5].x = v[i + 10].x = cos(rad);
        v[i + 5].y = v[i + 10].y = sin(rad);
        v[i + 5].z = .5;
        v[i + 10].z = -.5;
    }

    for (i = 0; i < 15; i++) add_vertex(m, coord(v[i].x, v[i].y, v[i].z));
    add_face(m, { 0, 5, 9 });
    add_face(m, { 1, 6, 5 });
    add_face(m, { 2, 7, 6 });
    add_face(m, { 3, 8, 7 });
    add_face(m, { 4, 9, 8 });

    add_face(m, { 0, 14, 10 });
    add_face(m, { 1, 10, 11 });
    add_face(m, { 2, 11, 12 });
    add_face(m, { 3, 12, 13 });
    add_face(m, { 4, 13, 14 });

    add_face(m, { 0, 10, 5 });
    add_face(m, { 1, 5, 10 });
    add_face(m, { 1, 11, 6 });
    add_face(m, { 2, 6, 11 });
    add_face(m, { 2, 12, 7 });
    add_face(m, { 3, 7, 12 });
    add_face(m, { 3, 13, 8 });
    add_face(m, { 4, 8, 13 });
    add_face(m, { 4, 14, 9 });
    add_face(m, { 0, 9, 14 });

    setNorm(m);

    aggregate_vertices(m);

    return m;
}

bool is_holeEdge(edge* e)
{
    /* fill in the blank */
    if ((e->f_list)->size() == 1) {
        return true;
    }
    else {
        return false;
    }
    return true; // delete this line after you fill in the blank.
}

bool is_holeVertex(vertex* v)
{
    if (v->f_list->size() != v->e_list->size()) {
        return true;
    }
    else {
        return false;
    }
    /* fill in the blank */
    return true; // delete this line after you fill in the blank.
}

vertex* face_point(face* f)
{
    if (f->face_pt != NULL) {
        return(f->face_pt);

    }
    else {
        int l = 0;
        //vertex* face_pt1 = new vertex();
        f->face_pt = vtx_init();
        f->face_pt->xyz.set(0., 0., 0.);
        for (int k = 0; k < f->v_list->size(); k++) {
            f->face_pt->xyz.add((f->v_list->at(k)->xyz));// (f->v_list->at(k)->xyz);
        }
        f->face_pt->xyz.div(static_cast<float>(f->v_list->size()));
        //f->face_pt = face_pt1;
        return(f->face_pt);
    }
    /* fill in the blank */
    return NULL; // delete this line after you fill in the blank.
}

vertex* edge_point(edge* e)
{
    if (e->edge_pt != NULL) {
        return e->edge_pt;
    }
    else {
        if (is_holeEdge(e)) {
            e->edge_pt = vtx_init();
            e->edge_pt->xyz.set(0., 0., 0.);
            e->edge_pt->xyz.add(e->v1->xyz);
            e->edge_pt->xyz.add(e->v2->xyz);
            e->edge_pt->xyz.div(2.0);
            return(e->edge_pt);
        }
        else {

            e->edge_pt = vtx_init();
            e->edge_pt->xyz.set(0., 0., 0.);
            e->edge_pt->xyz.add(e->v1->xyz);
            e->edge_pt->xyz.add(e->v2->xyz);
            for (int k = 0; k < e->f_list->size(); k++) {
                e->edge_pt->xyz.add((face_point(e->f_list->at(k)))->xyz);
            }
            e->edge_pt->xyz.div((2.0 + e->f_list->size()));
            return(e->edge_pt);

        }
    }
    /* fill in the blank */
    return NULL; // delete this line after you fill in the blank.
}


vertex* vertex_point(vertex* v)
{
    if (v->v_new != NULL) {
        return v->v_new;
    }
    else {
        if (is_holeVertex(v)) {
            v->v_new = vtx_init();
            v->v_new->xyz.set(0., 0., 0.);
            int cnt = 0;
            for (int k = 0; k < v->e_list->size(); k++) {
                if (is_holeEdge((v->e_list)->at(k))) {
                    cnt++; v->v_new->xyz.add(edge_point(v->e_list->at(k))->xyz);
                }

            }
            v->v_new->xyz.div(cnt);
            return v->v_new;
        }
        else {
            //avg_mid_point * 2
            v->v_new = vtx_init();
            v->v_new->xyz.set(0., 0., 0.);
            for (int k = 0; k < v->e_list->size(); k++) {
                v->v_new->xyz.add((v->e_list->at(k))->v1->xyz);
                v->v_new->xyz.add((v->e_list->at(k))->v2->xyz);
            }

            //v->v_new->xyz.mul(2.0);
            v->v_new->xyz.div(v->e_list->size());
            //avg_face_points
            coord test = coord(0.f, 0.f, 0.f);

            for (int k = 0; k < v->f_list->size(); k++) {
                test.add(face_point(v->f_list->at(k))->xyz);
            }
            test.div(v->f_list->size());
            v->v_new->xyz.add(test);

            //n-3 * v
            test = coord(0.f, 0.f, 0.f);
            test.add(v->xyz);
            test.mul(v->f_list->size() - 3);
            v->v_new->xyz.add(test);

            //#of face
            v->v_new->xyz.div(v->f_list->size());

            return v->v_new;

        }


    }
    /* fill in the blank */
    return NULL; // delete this line after you fill in the blank.
}
object* catmull_clark(object* obj)
{
    object* newobj = obj_init();


    /* fill in the blank */
    if (obj->vertices_per_face == 3) {

        newobj->vertices_per_face = 4;
        for (int i = 0; i < obj->f_list->size(); i++) {
            face* test = obj->f_list->at(i);



            vertex* vpa = vertex_point(test->v_list->at(0));
            vertex* vpb = vertex_point(test->v_list->at(1));
            vertex* vpc = vertex_point(test->v_list->at(2));
            //vertex* vpd = vertex_point(test->v_list->at(3));

            vertex* epab = (edge_point(find_edge(obj, test->v_list->at(0), test->v_list->at(1))));
            vertex* epbc = (edge_point(find_edge(obj, test->v_list->at(1), test->v_list->at(2))));
            vertex* epca = (edge_point(find_edge(obj, test->v_list->at(2), test->v_list->at(0))));
            //vertex* epda = (edge_point(find_edge(obj, test->v_list->at(3), test->v_list->at(0))));

            vertex* fpabc = (face_point(test));

            if (find_v(newobj, vpa) == NULL)
                vpa = add_vertex(newobj, vpa->xyz);
            else
                vpa = find_v(newobj, vpa);
            if (find_v(newobj, vpb) == NULL)
                vpb = add_vertex(newobj, vpb->xyz);
            else
                vpb = find_v(newobj, vpb);
            if (find_v(newobj, vpc) == NULL)
                vpc = add_vertex(newobj, vpc->xyz);
            else
                vpc = find_v(newobj, vpc);

            if (find_v(newobj, epab) == NULL)
                epab = add_vertex(newobj, epab->xyz);
            else
                epab = find_v(newobj, epab);
            if (find_v(newobj, epbc) == NULL)
                epbc = add_vertex(newobj, epbc->xyz);
            else
                epbc = find_v(newobj, epbc);
            if (find_v(newobj, epca) == NULL)
                epca = add_vertex(newobj, epca->xyz);
            else
                epca = find_v(newobj, epca);


            if (find_v(newobj, fpabc) == NULL)
                fpabc = add_vertex(newobj, fpabc->xyz);
            else
                fpabc = find_v(newobj, fpabc);



            add_face(newobj, { vpa->idx,epab->idx,fpabc->idx,epca->idx });
            add_face(newobj, { vpb->idx,epbc->idx,fpabc->idx,epab->idx });
            add_face(newobj, { vpc->idx,epca->idx,fpabc->idx,epbc->idx });
            //add_face(newobj, { vpd->idx,epda->idx,fpabc->idx,epcd->idx });
        }

    }
    else {
        newobj->vertices_per_face = 4;
        for (int i = 0; i < obj->f_list->size(); i++) {
            face* test = obj->f_list->at(i);



            vertex* vpa = vertex_point(test->v_list->at(0));
            vertex* vpb = vertex_point(test->v_list->at(1));
            vertex* vpc = vertex_point(test->v_list->at(2));
            vertex* vpd = vertex_point(test->v_list->at(3));

            vertex* epab = (edge_point(find_edge(obj, test->v_list->at(0), test->v_list->at(1))));
            vertex* epbc = (edge_point(find_edge(obj, test->v_list->at(1), test->v_list->at(2))));
            vertex* epcd = (edge_point(find_edge(obj, test->v_list->at(2), test->v_list->at(3))));
            vertex* epda = (edge_point(find_edge(obj, test->v_list->at(3), test->v_list->at(0))));

            vertex* fpabc = (face_point(test));

            if (find_v(newobj, vpa) == NULL)
                vpa = add_vertex(newobj, vpa->xyz);
            else
                vpa = find_v(newobj, vpa);

            if (find_v(newobj, vpb) == NULL)
                vpb = add_vertex(newobj, vpb->xyz);
            else
                vpb = find_v(newobj, vpb);

            if (find_v(newobj, vpc) == NULL)
                vpc = add_vertex(newobj, vpc->xyz);
            else
                vpc = find_v(newobj, vpc);

            if (find_v(newobj, vpd) == NULL)
                vpd = add_vertex(newobj, vpd->xyz);
            else
                vpd = find_v(newobj, vpd);

            if (find_v(newobj, epab) == NULL)
                epab = add_vertex(newobj, epab->xyz);
            else
                epab = find_v(newobj, epab);

            if (find_v(newobj, epbc) == NULL)
                epbc = add_vertex(newobj, epbc->xyz);
            else
                epbc = find_v(newobj, epbc);

            if (find_v(newobj, epcd) == NULL)
                epcd = add_vertex(newobj, epcd->xyz);
            else
                epcd = find_v(newobj, epcd);

            if (find_v(newobj, epda) == NULL)
                epda = add_vertex(newobj, epda->xyz);
            else
                epda = find_v(newobj, epda);

            if (find_v(newobj, fpabc) == NULL)
                fpabc = add_vertex(newobj, fpabc->xyz);
            else
                fpabc = find_v(newobj, fpabc);



            add_face(newobj, { vpa->idx,epab->idx,fpabc->idx,epda->idx });
            add_face(newobj, { vpb->idx,epbc->idx,fpabc->idx,epab->idx });
            add_face(newobj, { vpc->idx,epcd->idx,fpabc->idx,epbc->idx });
            add_face(newobj, { vpd->idx,epda->idx,fpabc->idx,epcd->idx });

        }
    }

    setNorm(newobj);

    aggregate_vertices(newobj);

    delete obj;

    return newobj;
}