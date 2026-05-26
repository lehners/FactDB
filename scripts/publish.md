```
git merge --squash --allow-unrelated-histories main
git commit --amend -m "Commit message" 

git remote add public git@github.com:lehners/FactDB.git
git push public public:main
```