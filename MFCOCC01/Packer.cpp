#include "Packer.h"
#include "pch.h"

#ifndef SHARED_HANDLERS
#include "MFCOCC01.h"
#endif

#include "MFCOCC01Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Packer::Packer(double w, double h) : root(0, 0, w, h), used(false) {}

void Packer::fit(std::vector<Node>& blocks) {
    Node* node;
    Node* block;
    auto blockItr = blocks.begin();
    while (blockItr != blocks.end()) {
        block = &(*blockItr);
        if ((node = findNode(&root, block->w, block->h)) != nullptr) {
            block->fit = splitNode(node, block->w, block->h);
            //CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
            //if (pMainFrame) {
            //    CMFCOCC01Doc* pDoc = dynamic_cast<CMFCOCC01Doc*>(pMainFrame->GetActiveDocument());
            //    pDoc->SendOutputMessage(_T("Il pannello %S è stato allocato."), block->name);
            //}
        }
        else {
            MessageBox(NULL, L"cannot allocate piece", L"lol", 0);
            //CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
            //if (pMainFrame) {
                //CMFCOCC01Doc* pDoc = dynamic_cast<CMFCOCC01Doc*>(pMainFrame->GetActiveDocument());
                //pDoc->SendOutputMessage(_T("Il pannello %S non ha spazio per essere allocato!"), block->name);
            //}
        }
        ++blockItr;
    }
}

Node* Packer::findNode(Node* root, double w, double h) {

    if (root->used) {
        Node* right = findNode(root->right, w, h);
        if (right != NULL) return right;
        Node* down = findNode(root->down, w, h);
        if (down != NULL) return down;
        return nullptr;
        //return (findNode(root->down, w, h) || findNode(root->right, w, h));
    }
    else if ((w <= root->w) && (h <= root->h)) {
        return root;
    }
    else {
        return NULL;
    }
}

Node* Packer::splitNode(Node* node, double w, double h) {
    node->used = true;
    node->down = new Node(node->x, node->y, node->w, node->h - h);
    node->right = new Node(node->x + w, node->y, node->w - w, h);
    return node;
}