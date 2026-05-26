import re

tables = ['dblp1','dblp2','dblp3','dblp4','dblp5','dblp6','dblp7','dblp8','dblp9','dblp10','dblp11','dblp12','dblp13','dblp14','dblp15','dblp16','dblp17','dblp18','dblp19','dblp20','dblp21','dblp22','dblp23','dblp24','dblp25','dblp26','dblp27','epinions75888','epinions75889','epinions75890','epinions75891','epinions75892','epinions75893','epinions75894','epinions75895','epinions75896','epinions75897','epinions75898','epinions75899','epinions75900','epinions75901','epinions75902','epinions75903','epinions75904','epinions75905','epinions75906','epinions75907','epinions75908','epinions75909','epinions75910','epinions75911','epinions75912','epinions75913','epinions75914','epinions75915','epinions75916','epinions75917','epinions75918','epinions75919','epinions75920','epinions75921','epinions75922','epinions75923','epinions75924','epinions75925','epinions75926','epinions75927','epinions75928','epinions75929','epinions75930','epinions75931','epinions75932','epinions75933','epinions75934','epinions75935','epinions75936','epinions75937','hetio45159','hetio45160','hetio45161','hetio45162','hetio45163','hetio45164','hetio45165','hetio45166','hetio45167','hetio45168','hetio45169','hetio45170','hetio45171','hetio45172','hetio45173','hetio45174','hetio45175','hetio45176','hetio45177','hetio45178','hetio45179','hetio45180','hetio45181','hetio45182','imdb1','imdb2','imdb3','imdb4','imdb5','imdb6','imdb7','imdb8','imdb9','imdb10','imdb11','imdb12','imdb13','imdb14','imdb15','imdb16','imdb17','imdb18','imdb19','imdb20','imdb21','imdb22','imdb23','imdb24','imdb25','imdb26','imdb27','imdb28','imdb29','imdb30','imdb31','imdb32','imdb33','imdb34','imdb35','imdb36','imdb37','imdb38','imdb39','imdb40','imdb41','imdb42','imdb43','imdb44','imdb45','imdb46','imdb47','imdb48','imdb49','imdb50','imdb51','imdb52','imdb53','imdb54','imdb55','imdb56','imdb57','imdb58','imdb59','imdb60','imdb61','imdb62','imdb63','imdb64','imdb65','imdb66','imdb67','imdb68','imdb69','imdb70','imdb71','imdb72','imdb73','imdb74','imdb75','imdb76','imdb77','imdb78','imdb79','imdb80','imdb81','imdb82','imdb83','imdb84','imdb85','imdb86','imdb87','imdb88','imdb89','imdb90','imdb91','imdb92','imdb93','imdb94','imdb95','imdb96','imdb97','imdb98','imdb99','imdb100','imdb101','imdb102','imdb103','imdb104','imdb105','imdb106','imdb107','imdb108','imdb109','imdb110','imdb111','imdb112','imdb113','imdb114','imdb115','imdb116','imdb117','imdb118','imdb119','imdb120','imdb121','imdb122','imdb123','imdb124','imdb125','imdb126','imdb127','watdiv1052572','watdiv1052573','watdiv1052574','watdiv1052575','watdiv1052576','watdiv1052577','watdiv1052578','watdiv1052579','watdiv1052580','watdiv1052581','watdiv1052582','watdiv1052583','watdiv1052584','watdiv1052585','watdiv1052586','watdiv1052587','watdiv1052588','watdiv1052589','watdiv1052590','watdiv1052591','watdiv1052592','watdiv1052593','watdiv1052594','watdiv1052595','watdiv1052596','watdiv1052597','watdiv1052598','watdiv1052599','watdiv1052600','watdiv1052601','watdiv1052602','watdiv1052603','watdiv1052604','watdiv1052605','watdiv1052606','watdiv1052607','watdiv1052608','watdiv1052609','watdiv1052610','watdiv1052611','watdiv1052612','watdiv1052613','watdiv1052614','watdiv1052615','watdiv1052616','watdiv1052617','watdiv1052618','watdiv1052619','watdiv1052620','watdiv1052621','watdiv1052622','watdiv1052623','watdiv1052624','watdiv1052625','watdiv1052626','watdiv1052627','watdiv1052628','watdiv1052629','watdiv1052630','watdiv1052631','watdiv1052632','watdiv1052633','watdiv1052634','watdiv1052635','watdiv1052636','watdiv1052637','watdiv1052638','watdiv1052639','watdiv1052640','watdiv1052641','watdiv1052642','watdiv1052643','watdiv1052644','watdiv1052645','watdiv1052646','watdiv1052647','watdiv1052648','watdiv1052649','watdiv1052650','watdiv1052651','watdiv1052652','watdiv1052653','watdiv1052654','watdiv1052655','watdiv1052656','watdiv1052657','yago0','yago1','yago2','yago3','yago4','yago5','yago6','yago7','yago8','yago9','yago10','yago11','yago12','yago13','yago14','yago15','yago16','yago17','yago18','yago19','yago20','yago21','yago22','yago23','yago24','yago25','yago26','yago27','yago28','yago29','yago30','yago31','yago32','yago33','yago34','yago35','yago36','yago37','yago38','yago39','yago40','yago41','yago42','yago43','yago44','yago45','yago46','yago47','yago48','yago49','yago50','yago51','yago52','yago53','yago54','yago55','yago56','yago57','yago58','yago59','yago60','yago61','yago62','yago63','yago64','yago65','yago66','yago67','yago68','yago69','yago70','yago71','yago72','yago73','yago74','yago75','yago76','yago77','yago78','yago79','yago80','yago81','yago82','yago83','yago84','yago85','yago86','yago87','yago88','yago89','yago90']

# print("create table isUnique(t varchar(100), c char(1), isDistinct bool, primary key(t, c));")
#
# for t in tables:
#     print(f"insert into isUnique (select '{t}' as table, 's' as column, count(distinct s)=count(*) as isUnique from {t});")
#     print(f"insert into isUnique (select '{t}' as table, 'd' as column, count(distinct d)=count(*) as isUnique from {t});")

# todo: check for each join if it is a candidate for a FK-PK join
unique =  [('imdb77', 'd', 't'), ('imdb83', 'd', 't'), ('watdiv1052574', 's', 't'), ('watdiv1052597', 's', 't'), ('imdb40', 'd', 't'), ('imdb54', 'd', 't'), ('imdb61', 'd', 't'), ('imdb99', 'd', 't'), ('watdiv1052585', 's', 't'), ('watdiv1052638', 's', 't'), ('yago49', 's', 't'), ('yago55', 's', 't'), ('yago61', 's', 't'), ('imdb5', 'd', 't'), ('imdb47', 'd', 't'), ('watdiv1052583', 'd', 't'), ('watdiv1052657', 's', 't'), ('dblp3', 's', 't'), ('dblp26', 's', 't'), ('imdb93', 'd', 't'), ('dblp4', 's', 't'), ('dblp7', 'd', 't'), ('imdb55', 'd', 't'), ('imdb56', 'd', 't'), ('imdb80', 'd', 't'), ('yago2', 'd', 't'), ('yago88', 'd', 't'), ('dblp19', 's', 't'), ('imdb17', 'd', 't'), ('imdb62', 'd', 't'), ('imdb97', 'd', 't'), ('watdiv1052572', 's', 't'), ('watdiv1052575', 's', 't'), ('watdiv1052586', 's', 't'), ('watdiv1052591', 's', 't'), ('watdiv1052604', 's', 't'), ('yago27', 's', 't'), ('imdb11', 'd', 't'), ('imdb15', 'd', 't'), ('imdb22', 'd', 't'), ('imdb46', 'd', 't'), ('imdb85', 'd', 't'), ('yago56', 's', 't'), ('yago57', 's', 't'), ('yago60', 's', 't'), ('yago90', 's', 't'), ('dblp6', 's', 't'), ('watdiv1052579', 's', 't'), ('watdiv1052598', 's', 't'), ('watdiv1052603', 's', 't'), ('watdiv1052605', 's', 't'), ('yago35', 's', 't'), ('dblp7', 's', 't'), ('dblp8', 's', 't'), ('dblp13', 's', 't'), ('imdb10', 'd', 't'), ('imdb32', 's', 't'), ('imdb33', 's', 't'), ('imdb64', 'd', 't'), ('imdb89', 'd', 't'), ('imdb92', 'd', 't'), ('watdiv1052609', 's', 't'), ('watdiv1052621', 's', 't'), ('watdiv1052646', 's', 't'), ('yago46', 's', 't'), ('yago67', 's', 't'), ('dblp9', 's', 't'), ('imdb19', 'd', 't'), ('imdb25', 'd', 't'), ('imdb26', 'd', 't'), ('imdb28', 'd', 't'), ('imdb29', 'd', 't'), ('imdb37', 'd', 't'), ('imdb41', 'd', 't'), ('watdiv1052611', 's', 't'), ('watdiv1052615', 's', 't'), ('watdiv1052630', 's', 't'), ('watdiv1052639', 's', 't'), ('yago83', 's', 't'), ('imdb12', 'd', 't'), ('imdb13', 'd', 't'), ('imdb86', 'd', 't'), ('imdb94', 'd', 't'), ('watdiv1052614', 's', 't'), ('watdiv1052623', 's', 't'), ('watdiv1052629', 's', 't'), ('watdiv1052641', 's', 't'), ('watdiv1052576', 's', 't'), ('watdiv1052624', 's', 't'), ('dblp11', 's', 't'), ('dblp17', 's', 't'), ('imdb4', 's', 't'), ('imdb6', 'd', 't'), ('imdb17', 's', 't'), ('imdb20', 's', 't'), ('imdb28', 's', 't'), ('imdb30', 'd', 't'), ('imdb31', 's', 't'), ('imdb51', 'd', 't'), ('watdiv1052583', 's', 't'), ('watdiv1052617', 's', 't'), ('watdiv1052626', 'd', 't'), ('watdiv1052632', 's', 't'), ('yago24', 's', 't'), ('yago65', 's', 't'), ('dblp14', 's', 't'), ('dblp15', 's', 't'), ('imdb14', 's', 't'), ('imdb81', 'd', 't'), ('imdb99', 's', 't'), ('watdiv1052590', 's', 't'), ('watdiv1052593', 's', 't'), ('watdiv1052637', 's', 't'), ('yago17', 's', 't'), ('yago25', 's', 't'), ('yago33', 's', 't'), ('yago63', 's', 't'), ('yago64', 's', 't'), ('yago90', 'd', 't'), ('imdb78', 'd', 't'), ('imdb88', 'd', 't'), ('watdiv1052657', 'd', 't'), ('yago16', 's', 't'), ('yago58', 's', 't'), ('yago59', 's', 't'), ('yago82', 'd', 't'), ('yago84', 's', 't'), ('yago86', 's', 't'), ('yago88', 's', 't'), ('yago89', 's', 't'), ('imdb16', 's', 't'), ('imdb39', 'd', 't'), ('yago36', 's', 't'), ('dblp16', 's', 't'), ('dblp18', 's', 't'), ('imdb14', 'd', 't'), ('watdiv1052588', 's', 't'), ('watdiv1052636', 'd', 't'), ('watdiv1052655', 'd', 't'), ('imdb8', 'd', 't'), ('imdb27', 'd', 't'), ('imdb65', 'd', 't'), ('imdb73', 'd', 't'), ('imdb87', 'd', 't'), ('imdb91', 'd', 't'), ('imdb98', 's', 't'), ('watdiv1052573', 's', 't'), ('watdiv1052601', 's', 't'), ('watdiv1052613', 's', 't'), ('watdiv1052614', 'd', 't'), ('yago1', 's', 't'), ('yago3', 's', 't'), ('yago26', 's', 't'), ('yago39', 's', 't'), ('yago75', 's', 't'), ('yago79', 's', 't'), ('yago84', 'd', 't'), ('dblp27', 's', 't'), ('imdb34', 'd', 't'), ('imdb58', 'd', 't'), ('imdb59', 'd', 't'), ('imdb66', 'd', 't'), ('imdb76', 'd', 't'), ('imdb98', 'd', 't'), ('watdiv1052618', 's', 't'), ('watdiv1052654', 's', 't'), ('yago70', 'd', 't'), ('yago87', 's', 't'), ('imdb16', 'd', 't'), ('imdb45', 'd', 't'), ('imdb82', 'd', 't'), ('watdiv1052607', 's', 't'), ('watdiv1052628', 's', 't'), ('watdiv1052635', 's', 't'), ('yago14', 's', 't'), ('dblp26', 'd', 't'), ('imdb7', 'd', 't'), ('imdb21', 'd', 't'), ('imdb30', 's', 't'), ('imdb68', 'd', 't'), ('imdb69', 'd', 't'), ('imdb77', 's', 't'), ('watdiv1052589', 's', 't'), ('watdiv1052636', 's', 't'), ('yago71', 's', 't'), ('watdiv1052649', 's', 't'), ('imdb36', 'd', 't'), ('imdb48', 'd', 't'), ('imdb50', 'd', 't'), ('imdb52', 'd', 't'), ('watdiv1052625', 'd', 't'), ('watdiv1052634', 'd', 't'), ('watdiv1052643', 's', 't'), ('yago82', 's', 't'), ('imdb60', 'd', 't'), ('imdb84', 'd', 't'), ('imdb33', 'd', 't'), ('watdiv1052589', 'd', 't'), ('watdiv1052645', 's', 't'), ('imdb44', 'd', 't'), ('watdiv1052596', 's', 't'), ('imdb9', 'd', 't'), ('imdb71', 'd', 't'), ('watdiv1052625', 's', 't'), ('watdiv1052656', 's', 't'), ('yago42', 's', 't'), ('yago70', 's', 't'), ('yago78', 's', 't'), ('imdb42', 'd', 't'), ('imdb49', 'd', 't'), ('watdiv1052577', 's', 't'), ('imdb57', 'd', 't'), ('imdb67', 'd', 't'), ('watdiv1052580', 's', 't'), ('watdiv1052595', 'd', 't'), ('yago9', 's', 't'), ('imdb24', 'd', 't'), ('imdb43', 'd', 't'), ('watdiv1052594', 's', 't'), ('watdiv1052599', 'd', 't'), ('watdiv1052612', 's', 't'), ('imdb31', 'd', 't'), ('imdb53', 'd', 't'), ('yago43', 's', 't'), ('imdb1', 'd', 't'), ('watdiv1052609', 'd', 't'), ('watdiv1052616', 's', 't'), ('watdiv1052628', 'd', 't'), ('watdiv1052640', 's', 't'), ('yago2', 's', 't'), ('watdiv1052647', 's', 't'), ('watdiv1052597', 'd', 't'), ('watdiv1052631', 's', 't'), ('watdiv1052606', 's', 't'), ('watdiv1052606', 'd', 't'), ('watdiv1052626', 's', 't'), ('yago8', 's', 't'), ('watdiv1052635', 'd', 't'), ('imdb35', 's', 't'), ('imdb72', 'd', 't'), ('imdb75', 'd', 't'), ('watdiv1052633', 's', 't'), ('yago51', 's', 't'), ('watdiv1052615', 'd', 't'), ('yago34', 's', 't'), ('imdb20', 'd', 't'), ('imdb23', 'd', 't'), ('imdb63', 'd', 't'), ('watdiv1052650', 's', 't'), ('imdb70', 'd', 't'), ('imdb95', 'd', 't'), ('watdiv1052592', 's', 't'), ('yago10', 's', 't'), ('yago45', 's', 't'), ('imdb74', 'd', 't'), ('watdiv1052648', 's', 't'), ('imdb18', 'd', 't'), ('yago52', 's', 't'), ('imdb32', 'd', 't'), ('imdb79', 'd', 't'), ('imdb90', 'd', 't'), ('imdb35', 'd', 't'), ('imdb96', 'd', 't'), ('imdb38', 'd', 't'), ('watdiv1052630', 'd', 't'), ('yago47', 's', 't'), ('watdiv1052582', 's', 't'), ('watdiv1052610', 's', 't'), ('watdiv1052620', 's', 't'), ('watdiv1052653', 's', 't'), ('yago28', 's', 't'),]

isUnique = {}
for [t, c, b] in unique:
    isUnique[(t, c)] = True if b=='t' else False

join_type_pattern = re.compile(r'(TI|BI)')
join_pattern = re.compile(r'n?([0-9a-zA-Z]*)(?:(_[0-9]+)?)_(s|d)==([0-9a-zA-Z]*)(?:(_[0-9]+)?)_(s|d)')
join_pattern2 = re.compile(r'struct[0-9]* \[label=')
regex3 = re.compile(r'struct[0-8]* \[label="\{ [a-zA-Z0-9]* [a-zA-Z0-9_]* \} "\];')
regex_queryname = re.compile(r"queryname: ([a-zA-Z_0-9.]*)")

queryname = None
ctr11 = 0
ctrn1 = 0
ctr1n = 0
ctrnn = 0
ctrAll = 0
tctr11 = 0
tctrn1 = 0
tctr1n = 0
tctrnn = 0
tctrAll = 0
countTopInserts = 0
countBottomInserts = 0
countTopInsertsAll = 0
countBottomInsertsAll = 0
querySomeBI = 0
queryAllBI = 0
queries = 0

def check_join(left_tbl, left_col, right_tbl, right_col):
    global ctr11, ctrn1, ctr1n, ctrnn, ctrAll
    def toString():
        return f"{left_tbl}.{left_col} == {right_tbl}.{right_col}"
    leftUnique = (left_tbl, left_col) in isUnique and isUnique[(left_tbl, left_col)]
    rightUnique = (right_tbl, right_col) in isUnique and isUnique[(right_tbl, right_col)]

    if leftUnique and rightUnique:
        #print(f"both unique: {toString()}")
        ctr11 += 1
    elif leftUnique:
        #print(f"left unique: {toString()}")
        ctr1n += 1
    elif rightUnique:
        #print(f"right unique: {toString()}")
        ctrn1 += 1
    else:
        #print(f"none unique: {toString()}")
        ctrnn += 1
    ctrAll += 1


with open("bench/results/plans.txt") as f:
    for line in f:
        matches = list(join_pattern.finditer(line))
        join_type = join_type_pattern.search(line)
        m2 = join_pattern2.search(line)
        m3 = regex3.search(line)
        matchQueryname = regex_queryname.match(line)
        if matchQueryname:
            if queryname:
                #if ctr11 == ctrAll and ctrAll != 0:
                if ctrnn == 0 and ctrAll != 0:
                    #    print(queryname)
                    #if ctrnn == ctrAll and ctrAll != 0:
                    # print(f'all unique {queryname}: {ctr11} {ctr1n} {ctrn1} {ctrnn}')
                    print(f'{queryname}')

            tctr11 += ctr11
            tctrn1 += ctrn1
            tctr1n += ctr1n
            tctrnn += ctrnn
            tctrAll += ctrAll

            ctr11 = 0
            ctrn1 = 0
            ctr1n = 0
            ctrnn = 0
            ctrAll = 0

            countBottomInsertsAll += countBottomInserts
            countTopInsertsAll += countTopInserts
            if countBottomInserts != 0:
                querySomeBI += 1
            if countTopInserts == 0 and countBottomInserts != 0:
                queryAllBI += 1
            queries += 1 if countTopInserts + countBottomInserts != 0 else 0
            countTopInserts = 0
            countBottomInserts = 0

            queryname = matchQueryname.group(1)
            # print(queryname)
        if matches:
            for m in matches:
                left_tbl = m.group(1).strip()
                left_col = m.group(3).strip()
                right_tbl = m.group(4).strip()
                right_col = m.group(6).strip()

                check_join(left_tbl, left_col, right_tbl, right_col)
        elif m2:
            pass
            # print(line.strip())
        if join_type:
            joinType = join_type.group(1).strip()
            if joinType == "TI":
                countTopInserts += 1
            elif joinType == "BI":
                countBottomInserts += 1



print(tctr11)
print(tctrn1)
print(tctr1n)
print(tctrnn)
print(tctrAll)

print(f"Queries with Some BI: {querySomeBI}")
print(f"Queries with All  BI: {queryAllBI}")
print(f"Queries             : {queries}")
print(f" total joins TI: {countTopInsertsAll}")
print(f" total joins BI: {countBottomInsertsAll}")

if __name__ == '__main__':
    pass