//
//  CXDocumentController.m
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXDocumentController.h"

@implementation CXDocumentController

+(CXDocumentController*)documentController
{
	return (CXDocumentController*)[NSDocumentController sharedDocumentController];
}

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	docToId = [NSMapTable weakToWeakObjectsMapTable];
	idToDoc = [NSMapTable weakToWeakObjectsMapTable];
	
	return self;
}

-(CXDocument*)documentWithId:(NSUInteger)docId
{
	return [idToDoc objectForKey:@(docId)];
}

-(NSUInteger)idOfDocument:(CXDocument *)document
{
	return [[docToId objectForKey:document] unsignedIntegerValue];
}

-(void)addDocument:(NSDocument *)document
{
	NSUInteger documentId = docToId.count + 1;
	NSNumber* key = @(documentId);
	[docToId setObject:key forKey:document];
	[idToDoc setObject:document forKey:key];
	[super addDocument:document];
}

-(void)removeDocument:(NSDocument *)document
{
	NSUInteger documentId = [[docToId objectForKey:document] unsignedIntegerValue];
	[docToId removeObjectForKey:document];
	[idToDoc removeObjectForKey:@(documentId)];
	[super removeDocument:document];
}

-(void)dealloc
{
	[docToId release];
	[idToDoc release];
	[super dealloc];
}

@end
