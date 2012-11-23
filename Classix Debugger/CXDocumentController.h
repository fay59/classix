//
//  CXDocumentController.h
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CXDocument.h"

@interface CXDocumentController : NSDocumentController
{
	NSMapTable* docToId;
	NSMapTable* idToDoc;
}

+(CXDocumentController*)documentController;

-(CXDocument*)documentWithId:(NSUInteger)docId;
-(NSUInteger)idOfDocument:(CXDocument*)document;

@end
