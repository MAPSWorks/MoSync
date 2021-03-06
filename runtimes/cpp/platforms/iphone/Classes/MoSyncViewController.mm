/* Copyright 2013 David Axmark

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#import "MoSyncViewController.h"

/**
 * Forward declaration.
 * Return a boolean value indicating whether the view controller supports the specified orientation.
 * Deprecated in iOS 6.0.
 * @param interfaceOrientation The orientation of the app’s user interface after the rotation.
 * @return YES if the view controller auto-rotates its view to the specified orientation, otherwise NO.
 */
BOOL MoSync_IsInterfaceOrientationSupported(UIInterfaceOrientation interfaceOrientation);

/**
 * Forward declaration.
 * Returns all of the interface orientations that the view controller supports.
 * Available in iOS 6.0 and later.
 * @return A mask with supported orientations.
 */
NSUInteger MoSync_SupportedInterfaceOrientations();

/**
 * Forward declaration.
 * Check if the current screen size has changed. If so send EVENT_TYPE_SCREEN_CHANGED event.
 * It's send only for non NativeUI applications. Once the NativeUI module is used
 * this event is not sent.
 * Usually the screen size changes when rotating device from portrait to landscape
 * and the other way around.
 * @param fromOrientation The old orientation of the user interface.
 */
void MoSync_OrientationChanged(UIInterfaceOrientation fromOrientation);

@interface MoSyncView : UIView <UITextFieldDelegate>
@end

@implementation MoSyncViewController

-(id)init
{
	self = [super init];
	if (self)
	{
		mosyncView = [[MoSyncView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
		self.view = mosyncView;
	}
	return self;
}

// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
	[super loadView];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (UIView*) getMoSyncView {
	return mosyncView;
}

/**
 * Return a boolean value indicating whether the view controller supports the specified orientation.
 * Deprecated in iOS 6.0.
 * @param interfaceOrientation The orientation of the app’s user interface after the rotation.
 * @return YES if the view controller auto-rotates its view to the specified orientation, otherwise NO.
 */
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return MoSync_IsInterfaceOrientationSupported(interfaceOrientation);
}

/**
 * Returns whether the view controller’s contents should auto rotate.
 * Available in iOS 6.0 and later.
 * @return YES or NO if there is only one orientation supported.
 */
-(BOOL)shouldAutorotate
{
    // If it supports only one orientation than is no need for autorotate
    NSUInteger orientations = MoSync_SupportedInterfaceOrientations();
    switch ( orientations )
    {
        case UIInterfaceOrientationMaskPortrait:
        case UIInterfaceOrientationMaskLandscapeLeft:
        case UIInterfaceOrientationMaskLandscapeRight:
        case UIInterfaceOrientationMaskPortraitUpsideDown:
            return NO;
    }
    return YES;
}

/**
 * Returns all of the interface orientations that the view controller supports.
 * Available in iOS 6.0 and later.
 * @return A mask with supported orientations.
 */
- (NSUInteger)supportedInterfaceOrientations
{
	NSUInteger orientations = MoSync_SupportedInterfaceOrientations();
    return orientations;
}

/**
 * Called after the user interface rotates.
 * @param fromInterfaceOrientation The old orientation of the user interface.
 */
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	MoSync_OrientationChanged(fromInterfaceOrientation);
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];

	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)dealloc {
    [super dealloc];
}

@end
